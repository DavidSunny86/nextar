/*
 * Shader.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "Pass.h"
#include "ShaderAsset.h"

namespace nextar {


	/*****************************************************/
	/* Shader											 */
	/*****************************************************/
	ShaderAsset::ShaderAsset(const StringID name) : nextar::Asset(name),
			parameterBufferSize(0),
			translucency(0) {
	}

	ShaderAsset::~ShaderAsset() {
	}

	ShaderAssetPtr ShaderAsset::Instance(Component::Factory* factory, const StringID name, const URL& loc,
			SharedComponent::Group* group) {
		ShaderAssetPtr shader = Assign(static_cast<ShaderAsset*>(factory->AsyncCreate(ShaderAsset::CLASS_ID, name)));
		if(shader)
			shader->SetAssetLocator(loc);
		if(group)
			shader->AddToGroup(group);
		return shader;
	}

	uint32 ShaderAsset::GetClassID() const {
		return CLASS_ID;
	}

	void ShaderAsset::NotifyAssetLoaded() {
		StreamRequest* creationParams = static_cast<StreamRequest*>(streamRequest);
		ContextObject::NotifyCreated();
		/* update programs */
		/* build with compilation options */
		StreamPassList &spl = creationParams->passes;
		passes.clear();
		passes.reserve(creationParams->passes.size());
		for(auto i = spl.begin(); i != spl.end(); ++i) {
			PassPtr p(CreatePass((*i), creationParams->compilationOpt));
			passes.push_back(std::move(p));
		}

		/* update */
		ContextObject::NotifyUpdated(reinterpret_cast<UpdateParamPtr>(creationParams));
		/* parameter buffer */
		_UpdatePasses();
		/* mark request as complete */
		creationParams->flags |= StreamRequest::COMPLETED;
		/* notify dependents */
		Asset::NotifyAssetLoaded();
		SetReady(true);
	}

	Pass* ShaderAsset::CreatePass(ShaderAsset::StreamPass& p, const String& co) {
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
				r->programs[i]->NotifyUpdated(reinterpret_cast<ContextObject::UpdateParamPtr>(&update));
			}
		}

		return r;
	}

	void ShaderAsset::NotifyAssetUnloaded() {
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

	void ShaderAsset::LoadImpl(StreamRequest* request, bool) {
		Loader loader(request);
		loader.Serialize();
	}

	void ShaderAsset::UnloadImpl(StreamRequest*, bool) {
		passes.clear();
	}

	void ShaderAsset::NotifyAssetUpdated() {
		ContextObject::NotifyUpdated(0);
		_UpdatePasses();
		Asset::NotifyAssetUpdated();
	}

	void ShaderAsset::_UpdatePasses() {
		uint32 numParams = 0;
		for(auto &p : passes) {
			numParams += p->GetCustomParamCount();
		}
		properties.reserve(numParams);
		// todo Can optimize here, because we have the count of individual
		// custom parameters, we can start iterating from there and not go
		// over all parameters.
		/* calculate the property buffer descriptor */
		uint32 byteOffset = 0;
		for(auto &p : passes) {
			p->SetParamIndex(properties.size());
			for(auto &cb : p->constantBuffers) {
				ShaderParamIterator it = cb->GetParamIterator();
				_ProcessParamIterator(it, byteOffset);
			}

			ShaderParamIterator it = p->GetSamplerIterator();
			_ProcessParamIterator(it, byteOffset);
		}

		parameterBufferSize = byteOffset;
	}

	void ShaderAsset::Create(nextar::RenderContext*) {
		// todo
	}

	void ShaderAsset::Update(RenderContext* rc, ContextObject::UpdateParamPtr streamRequest) {
		StreamRequest* creationParams = reinterpret_cast<StreamRequest*>(streamRequest);

		bool useFallback = false;
		for(auto &p : passes) {
			useFallback &= p->NotifyUpdated(rc);
		}

		if (useFallback)
			Warn("Shader compilation failed.");
		else
			Compile(rc);
	}

	void ShaderAsset::Destroy(nextar::RenderContext* rc) {
		Decompile(rc);
		for(auto& p : passes) {
			p->NotifyDestroyed(rc);
		}
	}

	nextar::StreamRequest* ShaderAsset::CreateStreamRequestImpl(bool load) {
		return NEX_NEW ShaderAsset::StreamRequest(this);
	}

	void ShaderAsset::DestroyStreamRequestImpl(nextar::StreamRequest*& request, bool load) {
		ShaderAsset::StreamRequest* req = static_cast<ShaderAsset::StreamRequest*>(request);
		NEX_DELETE req;
		request = nullptr;
	}

	/*****************************************************/
	/* Shader::StreamRequest							 */
	/*****************************************************/
	ShaderAsset::StreamRequest::StreamRequest(ShaderAsset* shader) : AssetStreamRequest(shader), currentPass(-1) {
	}

	ShaderAsset::StreamRequest::~StreamRequest() {
	}

	void ShaderAsset::StreamRequest::SetOptions(const String& options) {
		compilationOpt = options;
	}

	void ShaderAsset::StreamRequest::SetProgramSource(GpuProgram::Type type, const String& src) {
		passes[currentPass].programSources[type] = src;
	}

	void ShaderAsset::StreamRequest::AddParam(const String& name,
			const String& param, const String& description, const String& defaultValue,
			ParamDataType type) {
	}

	void ShaderAsset::StreamRequest::AddMacro(const String& name,
			const String& param, const String& description, bool defaultValue) {
	}

	void ShaderAsset::StreamRequest::BindDefaultTexture(const String& unitName, TextureBase* texture) {
		ShaderAsset* shader = static_cast<ShaderAsset*>(streamedObject);
		DefaultTextureUnitMap& defaultTextureUnits = passes[currentPass].defaultTextureUnits;
		DefaultTextureUnitMap::iterator it = defaultTextureUnits.find(unitName);
		if (it == defaultTextureUnits.end()) {
			Error("Bind point out of bounds for default texture in shader: " + shader->GetName());
			return;
		}

		(*it).second.defaultTexture = texture;
		if (texture->IsTextureAsset())
			metaInfo.AddDependency(static_cast<TextureAsset*>(texture));
	}

	void ShaderAsset::StreamRequest::AddTextureUnit(const String& unitName, TextureUnitParams& tu) {
		DefaultTextureUnitMap& defaultTextureUnits = passes[currentPass].defaultTextureUnits;
		defaultTextureUnits[unitName].params = tu;
	}

	void ShaderAsset::StreamRequest::SetBlendState(BlendState& state) {
		passes[currentPass].blendState = state;
	}

	void ShaderAsset::StreamRequest::SetRasterState(RasterState& state) {
		passes[currentPass].rasterState = state;
	}

	void ShaderAsset::StreamRequest::SetDepthStencilState(DepthStencilState& state) {
		passes[currentPass].depthStencilState = state;
	}

	void ShaderAsset::StreamRequest::AddPass(const String& name) {
		currentPass = passes.size();
		passes.resize(currentPass+1);
		passes[currentPass].name = name;
	}
	/*****************************************************/
	/* Shader::Loader       							 */
	/*****************************************************/
	NEX_IMPLEMENT_FACTORY(ShaderAsset::Loader);
	ShaderAsset::Loader::Loader(nextar::StreamRequest* shader) : shaderRequest(shader) {
	}

	ShaderAsset::Loader::~Loader() {
	}

	void ShaderAsset::Loader::Serialize() {
		ShaderAsset* shaderPtr = static_cast<ShaderAsset*>(shaderRequest->GetStreamedObject());
		const URL& location = shaderPtr->GetAssetLocator();
		String ext = location.GetExtension();
		StringUtils::ToUpper(ext);
		ShaderAsset::LoaderImpl* impl = GetImpl(ext);
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
