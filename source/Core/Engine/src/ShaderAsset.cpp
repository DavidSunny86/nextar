/*
 * Shader.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <Pass.h>
#include <ShaderAsset.h>
#include <ConstantBuffer.h>
#include <ParameterIterator.h>

namespace nextar {


	/*****************************************************/
	/* Shader											 */
	/*****************************************************/
	ShaderAsset::ShaderAsset(const StringID name) : nextar::Asset(name)
	,translucency(0)
	,singlePassShader(nullptr) {
	}

	ShaderAsset::~ShaderAsset() {
		_DestroyPasses();
	}

	void ShaderAsset::_DestroyPasses() {
		if (singlePassShader)
			NEX_DELETE(singlePassShader);
		for(auto &p : passes)
			NEX_DELETE(p);
		passes.clear();
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
		_DestroyPasses();
		
		StreamPassList &spl = creationParams->passes;
		if (spl.size() > 1) {
			passes.reserve(spl.size());
			for(auto i = spl.begin(); i != spl.end(); ++i) {
				PassPtr p(CreatePass((*i), creationParams->compilationOpt));
				passes.push_back(std::move(p));
			}
		} else {
			singlePassShader = CreatePass(spl[0], creationParams->compilationOpt);
		}
		
		/* update */
		ContextObject::NotifyUpdated(reinterpret_cast<UpdateParamPtr>(creationParams));
		_BuildParameterTable();
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
		r->textureDescMap.swap(p.textureStates);
		//r->defaultTextureUnits.swap(p.defaultTextureUnits);
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
		Pass **it, **en;
		if(singlePassShader) {
			it = &singlePassShader;
			en = it + 1;
		} else {
			it = passes.data();
			en = it + passes.size();
		}

		for(; it != en; ++it) {
			for(uint32 i = 0; i < Pass::NUM_STAGES; ++i)
				if ((*it)->programs[i])
					(*it)->programs[i]->NotifyDestroyed();
		}

		_DestroyPasses();
		/* notify dependents */
		Asset::NotifyAssetUnloaded();
	}

	void ShaderAsset::LoadImpl(StreamRequest* request, bool) {
		Loader loader(request);
		loader.Serialize();
	}

	void ShaderAsset::UnloadImpl(StreamRequest*, bool) {
		_DestroyPasses();
	}

	void ShaderAsset::NotifyAssetUpdated() {
		ContextObject::NotifyUpdated(0);
		_BuildParameterTable();
		Asset::NotifyAssetUpdated();
	}

	void ShaderAsset::Create(nextar::RenderContext*) {
		// todo
	}

	void ShaderAsset::Update(RenderContext* rc, ContextObject::UpdateParamPtr streamRequest) {
		StreamRequest* creationParams = reinterpret_cast<StreamRequest*>(streamRequest);
		Pass **it, **en;
		bool useFallback = false;
		if(singlePassShader) {
			it = &singlePassShader;
			en = it + 1;
		} else {
			it = passes.data();
			en = it + passes.size();
		}

		for(; it != en; ++it) {
			useFallback &= (*it)->NotifyUpdated(rc);
		}

		if (useFallback)
			Warn("Shader compilation failed.");
		else
			Compile(rc);
	}

	void ShaderAsset::Destroy(nextar::RenderContext* rc) {
		Pass **it, **en;
		if(singlePassShader) {
			it = &singlePassShader;
			en = it + 1;
		} else {
			it = passes.data();
			en = it + passes.size();
		}
		Decompile(rc);
		for(; it != en; ++it) {
			(*it)->NotifyDestroyed(rc);
		}
	}

	nextar::StreamRequest* ShaderAsset::CreateStreamRequestImpl(bool load) {
		return NEX_NEW(ShaderAsset::StreamRequest(this));
	}

	void ShaderAsset::DestroyStreamRequestImpl(nextar::StreamRequest*& request, bool load) {
		ShaderAsset::StreamRequest* req = static_cast<ShaderAsset::StreamRequest*>(request);
		NEX_DELETE(req);
		request = nullptr;
	}

	void ShaderAsset::_BeginPass(PassPtr p, ParamTableBuilder& ptb) {
		p->passParamByteOffset = ptb.passParamOffset;
		p->materialParamByteOffset = ptb.materialParamOffset;
		p->objectParamByteOffset = ptb.objectParamOffset;
	}

	void ShaderAsset::_Process(ShaderParamIterator& it, ParamTableBuilder& ptb) {
		auto desc = (*it);
		if (desc.autoName != AutoParamName::AUTO_CUSTOM_CONSTANT)
			return;
		uint32 offset;
		switch(desc.frequency) {
		case UpdateFrequency::PER_FRAME:
			ptb.frameParamCount++;break;
		case UpdateFrequency::PER_VIEW:
			ptb.viewParamCount++;break;
		case UpdateFrequency::PER_MATERIAL:
			offset = ptb.materialParamOffset;
			ptb.materialParamOffset += desc.size;
			ptb.materialParamCount++;break;
		case UpdateFrequency::PER_OBJECT_INSTANCE:
			offset = ptb.objectParamOffset;
			ptb.objectParamOffset += desc.size;
			ptb.objectParamCount++;break;
		case UpdateFrequency::PER_PASS:
			offset = ptb.passParamOffset;
			ptb.passParamOffset += desc.size;
			ptb.passParamCount++;break;
		}

		ParamEntry pe;
		pe.descriptor = &desc;
		pe.offset = offset;
		ptb.passTable.push_back(pe);
	}

	void ShaderAsset::_EndPass(PassPtr& p, ParamTableBuilder& ptb) {

	}

	void ShaderAsset::_Finalize(ParamTableBuilder& ptb) {
		std::sort(ptb.passTable.begin(),ptb.passTable.end(), [] (const ParamEntry& first, const ParamEntry& second) -> bool {
			return first.descriptor->frequency < second.descriptor->frequency;
		});

		paramLookup.reserve(ptb.passTable.size());
		paramLookup.assign(ptb.passTable.begin(), ptb.passTable.end());

		uint32 offset = ptb.frameParamCount + ptb.viewParamCount;
		// todo Only automatic frame and view parameters are supported currently
		NEX_ASSERT(offset);
		passProperties.first = paramLookup.begin() + offset;
		offset += ptb.passParamCount;
		materialProperties.first = passProperties.second = paramLookup.begin() + offset;
		offset += ptb.materialParamCount;
		objectProperties.first = materialProperties.second = paramLookup.begin() + offset;
		offset += ptb.objectParamCount;
		objectProperties.second = paramLookup.begin() + offset;
	}

	void ShaderAsset::_BuildParameterTable() {
		paramLookup.clear();

		ParamTableBuilder paramTableBuilder;
		Pass **it, **en;
		if(singlePassShader) {
			it = &singlePassShader;
			en = it + 1;
		} else {
			it = passes.data();
			en = it + passes.size();
		}
		for(; it != en; ++it) {
			PassPtr p = (*it);
			_BeginPass(p, paramTableBuilder);
			ConstantBufferList& cbl = p->GetConstantBuffers();
			for(auto &c : cbl) {
				ShaderParamIterator it = c->GetParamIterator();
				while(it) {
					_Process(it, paramTableBuilder);
					++it;
				}
			}
			_EndPass(p, paramTableBuilder);
			ShaderParamIterator it = p->GetSamplerIterator();
			while(it) {
				_Process(it, paramTableBuilder);
				++it;
			}
		}
		_Finalize(paramTableBuilder);
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
	
	void ShaderAsset::StreamRequest::AddTextureUnit(const String& unitName, TextureUnitParams& tu, TextureBase* texture) {
		ShaderAsset* shader = static_cast<ShaderAsset*>(streamedObject);
		Pass::TextureDescMap& defaultTextureUnits = passes[currentPass].textureStates;
		Pass::SamplerDesc& sd = defaultTextureUnits[unitName];
		sd.texUnitParams = tu;
		sd.defaultTexture = texture;
		if (texture->IsTextureAsset())
			metaInfo.AddDependency(static_cast<TextureAsset*>(texture));
	}

	ParameterIterator ShaderAsset::GetParameterIterator(UpdateFrequency type) {
		return ParameterIterator(passes, type);
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
		currentPass = (uint32)passes.size();
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
