/*
 * Shader.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "Pass.h"
#include "Shader.h"

namespace nextar {


	/*****************************************************/
	/* Shader											 */
	/*****************************************************/
	Shader::Shader(AssetManager* manager, const String& name) : Asset(manager, name),
			translucency(0), shaderParamBufferSize(0) {
	}

	Shader::~Shader() {
	}

	Shader* Shader::Instance(AssetManager* manager, const String& name, const URL& loc) {
		Shader* shader = static_cast<Shader*>(manager->AsyncCreate(Shader::TYPE, name));
		if(shader)
			shader->SetAssetLocator(loc);
		return shader;
	}

	int Shader::GetType() const {
		return TYPE;
	}

	void Shader::NotifyAssetLoaded() {
		StreamRequest* creationParams = static_cast<StreamRequest*>(streamRequest);
		ContextObject::NotifyCreated();
		/* update programs */
		/* build compilation options */
		String compilationOpt;
		const String& name = GetName();
		size_t pos = name.find_last_of(':');

		if (pos == String::npos) {
			Warn("No compilation options set. Using defaults.");
			MacroTable& mt = creationParams->macroTable;
			for (auto m : mt) {
				if(m.value)
					StringUtils::PushBackWord(compilationOpt, m.macro);
			}
		} else {
			MacroTable& mt = creationParams->macroTable;
			for(size_t i = pos+1; i < name.size(); ++i) {
				uint8 index = name[i]-1;
				NEX_ASSERT(index < mt.size());
				StringUtils::PushBackWord(compilationOpt, mt[index].macro);
			}
		}

		StreamPassList &spl = creationParams->passes;
		passes.clear();
		passes.reserve(creationParams->passes.size());
		for(auto i = spl.begin(); i != spl.end(); ++i) {
			PassPtr p(CreatePass((*i), compilationOpt));
			passes.push_back(std::move(p));
		}

		/* update */
		ContextObject::NotifyUpdated(creationParams);
		/* parameter buffer */
		_UpdatePasses();
		/* mark request as complete */
		creationParams->flags |= StreamRequest::COMPLETED;
		/* notify dependents */
		Asset::NotifyAssetLoaded();
		SetReady(true);
	}

	Pass* Shader::CreatePass(Shader::StreamPass& p, const String& co) {
		Pass* r = CreatePassImpl(p.name);
		r->blendState = p.blendState;
		r->depthStencilState = p.depthStencilState;
		r->rasterState = p.rasterState;
		r->defaultTextureUnits.swap(p.defaultTextureUnits);
		for(uint32 i = 0; i < Pass::NUM_STAGES; ++i) {
			GpuProgram::Type t = (GpuProgram::Type)i;
			const String& source = p.programSources[i];
			if (source.length()) {
				r->programs[i] = GpuProgram::Instance(t);
				GpuProgram::UpdateParam update = {
						source.c_str(),
						&co
				};
				r->programs[i]->NotifyUpdated(&update);
			}
		}

		return r;
	}

	void Shader::NotifyAssetUnloaded() {
		ContextObject::NotifyDestroyed();

		for(auto &p : passes) {
			for(uint32 i = 0; i < Pass::NUM_STAGES; ++i)
				if (p->programs[i])
					p->programs[i]->NotifyDestroyed();
		}

		passes.clear();
		/* notify dependents */
		Asset::NotifyAssetUnloaded();
	}

	void Shader::LoadImpl(StreamRequest* request, bool) {
		Loader loader(request);
		loader.Serialize();
	}

	void Shader::UnloadImpl(StreamRequest*, bool) {
		passes.clear();
	}

	void Shader::NotifyAssetUpdated() {
		ContextObject::NotifyUpdated(nullptr);
		_UpdatePasses();
		Asset::NotifyAssetUpdated();
	}

	void Shader::_UpdatePasses() {
		for(auto &p : passes) {
			p->FinalizeUpdate();
		}
	}

	void Shader::Create(nextar::RenderContext*) {
		// @todo
	}

	void Shader::Update(RenderContext* rc, ContextObject::UpdateParamPtr streamRequest) {
		StreamRequest* creationParams = static_cast<StreamRequest*>(streamRequest);

		bool useFallback = false;
		for(auto &p : passes) {
			useFallback &= p->NotifyUpdated(rc);
		}

		if (useFallback)
			Warn("@todo Shader compilation failed.");
		else
			Compile(rc);
	}

	void Shader::Destroy(nextar::RenderContext* rc) {
		Decompile(rc);
		for(auto& p : passes) {
			p->NotifyDestroyed(rc);
		}
	}

	nextar::StreamRequest* Shader::CreateStreamRequestImpl(bool load) {
		return NEX_NEW Shader::StreamRequest(this);
	}

	void Shader::DestroyStreamRequestImpl(nextar::StreamRequest*& request, bool load) {
		Shader::StreamRequest* req = static_cast<Shader::StreamRequest*>(request);
		NEX_DELETE req;
		request = nullptr;
	}

	/*****************************************************/
	/* Shader::StreamRequest							 */
	/*****************************************************/
	Shader::StreamRequest::StreamRequest(Shader* shader) : AssetStreamRequest(shader), currentPass(-1) {
	}

	Shader::StreamRequest::~StreamRequest() {
	}

	void Shader::StreamRequest::SetProgramSource(GpuProgram::Type type, const String& src) {
		passes[currentPass].programSources[type] = src;
	}

	void Shader::StreamRequest::AddParam(const String& name,
			const String& param, const String& description, const String& defaultValue,
			ParamDataType type) {
	}

	void Shader::StreamRequest::AddMacro(const String& name,
			const String& param, const String& description, bool defaultValue) {

		if (macroTable.size() >= 255) {
			Error("Too many macros in one shader!.");
			return;
		}
		macroTable.push_back({defaultValue, param});
	}

	void Shader::StreamRequest::BindDefaultTexture(const String& unitName, TextureBase* texture) {
		Shader* shader = static_cast<Shader*>(streamedObject);
		DefaultTextureUnitMap& defaultTextureUnits = passes[currentPass].defaultTextureUnits;
		DefaultTextureUnitMap::iterator it = defaultTextureUnits.find(unitName);
		if (it == defaultTextureUnits.end()) {
			Error("Bind point out of bounds for default texture in shader: " + shader->GetName());
			return;
		}

		(*it).second.defaultTexture = texture;
		if (texture->IsTextureAsset())
			metaInfo.AddDependency(static_cast<Texture*>(texture));
	}

	void Shader::StreamRequest::AddTextureUnit(const String& unitName, TextureUnitParams& tu) {
		DefaultTextureUnitMap& defaultTextureUnits = passes[currentPass].defaultTextureUnits;
		defaultTextureUnits[unitName].params = tu;
	}

	void Shader::StreamRequest::SetBlendState(BlendState& state) {
		passes[currentPass].blendState = state;
	}

	void Shader::StreamRequest::SetRasterState(RasterState& state) {
		passes[currentPass].rasterState = state;
	}

	void Shader::StreamRequest::SetDepthStencilState(DepthStencilState& state) {
		passes[currentPass].depthStencilState = state;
	}

	void Shader::StreamRequest::AddPass(const String& name) {
		currentPass = passes.size();
		passes.resize(currentPass+1);
		passes[currentPass].name = name;
	}
	/*****************************************************/
	/* Shader::Loader       							 */
	/*****************************************************/
	NEX_IMPLEMENT_FACTORY(Shader::Loader);
	Shader::Loader::Loader(nextar::StreamRequest* shader) : shaderRequest(shader) {
	}

	Shader::Loader::~Loader() {
	}

	void Shader::Loader::Serialize() {
		Shader* shaderPtr = static_cast<Shader*>(shaderRequest->GetStreamedObject());
		const URL& location = shaderPtr->GetAssetLocator();
		String ext = location.GetExtension();
		StringUtils::ToUpper(ext);
		Shader::LoaderImpl* impl = GetImpl(ext);
		if (!impl) {
			Error("No shader compiler registered.");
			NEX_THROW_GracefulError(EXCEPT_MISSING_PLUGIN);
		}

		InputStreamPtr input = FileSystem::Instance().OpenRead(location);

		if (input) {
			impl->Load(input, *this);
		} else {
			Error(
					String("Could not open shader file: ")
							+ shaderPtr->GetAssetLocator().ToString());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOCATE_ASSET);
		}

	}
} /* namespace nextar */
