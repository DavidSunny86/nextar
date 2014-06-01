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
#include <RenderManager.h>

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

	uint32 ShaderAsset::GetClassID() const {
		return CLASS_ID;
	}

	void ShaderAsset::NotifyAssetLoaded() {
		StreamRequest* creationParams = static_cast<StreamRequest*>(streamRequest);
		/* update programs */
		/* build with compilation options */
		_DestroyPasses();
		
		StreamPassList &spl = creationParams->passes;
		if (spl.size() > 1) {
			passes.reserve(spl.size());
			for(auto i = spl.begin(); i != spl.end(); ++i) {
				PassPtr p(_CreatePass((*i), creationParams->compilationOpt));
				passes.push_back(std::move(p));
			}
		} else {
			singlePassShader = _CreatePass(spl[0], creationParams->compilationOpt);
		}
		
		/* update */
		_BuildParameterTable(spl);
		/* mark request as complete */
		creationParams->flags |= StreamRequest::COMPLETED;
		/* notify dependents */
		Asset::NotifyAssetLoaded();
		SetReady(true);
	}

	Pass* ShaderAsset::_CreatePass(ShaderAsset::StreamPass& p, const String& co) {
		Pass* r = NEX_NEW( Pass(p.name) );
		p.compileParams = &co;
		r->RequestUpdate(Pass::MSG_PASS_COMPILE, reinterpret_cast<ContextObject::ContextParamPtr>(&p.compileParams));
		return r;
	}

	void ShaderAsset::NotifyAssetUnloaded() {
		_DestroyPasses();
		/* notify dependents */
		Asset::NotifyAssetUnloaded();
	}

	void ShaderAsset::UnloadImpl(nextar::StreamRequest*, bool) {
		_DestroyPasses();
	}

	void ShaderAsset::NotifyAssetUpdated() {
		// _BuildParameterTable();
		Asset::NotifyAssetUpdated();
	}

	nextar::StreamRequest* ShaderAsset::CreateStreamRequestImpl(bool load) {
		return NEX_NEW(ShaderAsset::StreamRequest(this));
	}

	void ShaderAsset::DestroyStreamRequestImpl(nextar::StreamRequest*& request, bool load) {
		ShaderAsset::StreamRequest* req = static_cast<ShaderAsset::StreamRequest*>(request);
		NEX_DELETE(req);
		request = nullptr;
	}

	void ShaderAsset::_BeginPass(Pass::OffsetParams& p, ParamTableBuilder& ptb) {
		p.passParamByteOffset = ptb.passParamOffset;
		p.materialParamByteOffset = ptb.materialParamOffset;
		p.objectParamByteOffset = ptb.objectParamOffset;
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

	void ShaderAsset::_EndPass(PassPtr& p, Pass::OffsetParams& offsets, ParamTableBuilder& ptb) {
		p->RequestUpdate(Pass::MSG_PASS_UPDATE_OFFSET, static_cast<const Pass::OffsetParams*>(&offsets));
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

	void ShaderAsset::_BuildParameterTable(StreamPassList& spl) {
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
		uint32 index = 0;
		for(; it != en; ++it, ++index) {
			PassPtr p = (*it);
			_BeginPass(spl[index].offsets, paramTableBuilder);
			ConstantBufferList& cbl = p->GetConstantBuffers();
			for(auto &c : cbl) {
				ShaderParamIterator it = c->GetParamIterator();
				while(it) {
					_Process(it, paramTableBuilder);
					++it;
				}
			}
			_EndPass(p, spl[index].offsets, paramTableBuilder);
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

	void ShaderAsset::StreamRequest::SetProgramSource(GpuProgram::Type type, String&& src) {
		passes[currentPass].compileParams.programSources[type] = std::move(src);
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
		Pass::TextureDescMap& defaultTextureUnits = passes[currentPass].compileParams.textureStates;
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
		passes[currentPass].compileParams.blendState = state;
	}

	void ShaderAsset::StreamRequest::SetRasterState(RasterState& state) {
		passes[currentPass].compileParams.rasterState = state;
	}

	void ShaderAsset::StreamRequest::SetDepthStencilState(DepthStencilState& state) {
		passes[currentPass].compileParams.depthStencilState = state;
	}

	void ShaderAsset::StreamRequest::AddPass(StringID name) {
		currentPass = (uint32)passes.size();
		passes.resize(currentPass+1);
		passes[currentPass].name = name;
	}

	void ShaderAsset::StreamRequest::SetParamterBuffer(ParameterBuffer&& data) {
		ShaderAsset* shader = static_cast<ShaderAsset*>(streamedObject);
		shader->passParamData = std::move(data);
	}
} /* namespace nextar */
