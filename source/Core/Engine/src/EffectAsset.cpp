/*
 * EffectAsset.cpp
 *
 *  Created on: 04-Mar-2017
 *      Author: obhi
 */

#include <EffectAsset.h>
#include <RenderSystem.h>
#include <RenderManager.h>

namespace nextar {

//****************************************

EffectAsset::SourceLoadRequest::SourceLoadRequest(EffectAsset *asset) :
	AssetStreamRequest(asset) {
}

void EffectAsset::SourceLoadRequest::AddTags(const StringUtils::WordList & tags) {
	ConstMultiStringHelper cms(tags);

	_data.tags.reserve(cms.Length());
	auto it = cms.Iterate();
	String v;
	
	while (it.HasNext(v))
		AddTag(StringUtils::Hash(v));
}

void EffectAsset::SourceLoadRequest::SetProgramSource(
	PipelineStage::Name stage, String&& src) {
	_data.programSources[stage] = std::move(src);
}

void EffectAsset::SourceLoadRequest::SetSemanticMap(
		VarToAutoParamMap&& m) {
	_data.autoNames = std::move(m);
}

void EffectAsset::SourceLoadRequest::AddSamplerUnit(TextureUnitParams& unit,
	StringUtils::WordList&& boundUnitNames) {
	_data.textureStates.push_back(SamplerDesc());
	SamplerDesc& sd = _data.textureStates.back();
	sd.texUnitParams = unit;
	sd.unitsBound = std::move(boundUnitNames);
}

void EffectAsset::SourceLoadRequest::AddAutoNameMapping(
		const String& varName, AutoParamName name) {
	_data.autoNames[StringUtils::Hash(varName)] = name;
}

//****************************************

EffectAsset::FullLoadRequest::FullLoadRequest(EffectAsset* owner) :
SourceLoadRequest(owner),
renderQueueFlags(0) {
}

EffectAsset::FullLoadRequest::~FullLoadRequest() {
}

//****************************************
//            EffectAsset
//****************************************
EffectAsset::EffectAsset(const StringID name, const StringID factory) :
Asset(name, factory), acquireRef(0) {
}

EffectAsset::~EffectAsset() {
}

void EffectAsset::ResolveMaterial(
		const StringUtils::WordList& options,
		Material& m) {

	RenderSystemPtr renderSys = RenderManager::Instance().GetActiveRenderSystem();
	const String& baseOptions = RenderManager::Instance().GetGlobalShaderOptions();
	uint32 passCount = renderSys->GetPassCount();

	ShaderOptions shaderOptions;
	String strOptions;
	shaderOptions.Append(baseOptions);
	shaderOptions.Append(options);

	if (!m._reserved_p) {
		m._reserved_p = RenderManager::Instance().AllocMaterialRenderInfo();
		m.flags |= Material::RENDER_INFO_PER_PASS;
	}

	const ShaderData* data = _GetShaderData();
	if (!data)
		return;

	RenderInfo_Material* rinfo = static_cast<RenderInfo_Material*>(m._reserved_p);
	uint32 count = renderSys->GetPassCount();
	for (uint32 i = 0; i < count; ++i) {
		RenderPass* pass = renderSys->GetPass(i);
		RenderPass::Info info = pass->GetPassInfo();
		if (_CanCompile(*data, info)) {
			// we can call @_Resolve with the effect from this pass
			// but we do not do that for now.
			rinfo[i].shaderUnit = -1;
		} else {

			ShaderOptions newOptions(info._options ? *info._options : StringUtils::Null);
			newOptions.Append(shaderOptions);
			newOptions.ToString(strOptions);
			rinfo[i].shaderUnit = _Resolve(strOptions);
		}
	}
	// combine options
	// for each render pass
	//   combine options
	//   find hash, if not available
	//     generate shader
	//   use shader to fill render info

}

void EffectAsset::ResolveMaterialSingle(const StringUtils::WordList & options, 
	const StringUtils::WordList & renderPassOptions, 
	Material & m) {

	const String& baseOptions = RenderManager::Instance().GetGlobalShaderOptions();

	ShaderOptions shaderOptions;
	String strOptions;
	shaderOptions.Append(baseOptions);
	shaderOptions.Append(options);
	shaderOptions.Append(renderPassOptions);
	shaderOptions.ToString(strOptions);
	m._reserved_i = _Resolve(strOptions);
	// combine options
	// for each render pass
	//   combine options
	//   find hash, if not available
	//     generate shader
	//   use shader to fill render info

}

void EffectAsset::AsyncAcquireData() {
	if (!acquireRef.fetch_add(1)) {
		std::lock_guard<spinlock_mutex> g(_lock);
		// reset the asset state
		assetState = ASSET_CREATED;
		// create a load request object
		loadRequest = WasLoaded() ? NEX_NEW(SourceLoadRequest(this)) : NEX_NEW(FullLoadRequest(this));
		// request a load
		AsyncRequestLoad();
	}
}

void EffectAsset::AsyncReleaseData() {
	if (!acquireRef.fetch_sub(1) == 1) {
		std::lock_guard<spinlock_mutex> g(_lock);
		if (loadRequest) {
			NEX_DELETE(loadRequest);
		}
	}
}

StreamRequest* EffectAsset::CreateStreamRequestImpl(bool load) {
	if (load)
		return loadRequest;
	return nullptr;
}

bool EffectAsset::_CanCompile(const ShaderData & data, const RenderPass::Info & info) {
	if (info.flags & RenderPass::PASS_OVERRIDES_MATERIAL)
		return false;
	if (info.tag == 0 || std::find(data.tags.begin(), data.tags.end(), info.tag) != data.tags.end())
		return true;
	return false;
}

int32 EffectAsset::_Resolve(const StringUtils::WordList & options) {
	hash_t h = StringUtils::Hash(options);
	int32 unit = _FindUnit(options, h);
	if (unit < 0) {
		unit = _CreateUnit(options, h, options);
	}
	return unit;
}

EffectAsset::ShaderData* EffectAsset::_GetShaderData() {
	if (this->loadRequest)
		return &this->loadRequest->GetData();
	return nullptr;
}

int32 EffectAsset::_FindUnit(const String & id, hash_t h) {
	int32 n = (int32)shaderUnits.size();
	for (int32 i = 0; i < n; ++i) {
		auto& su = shaderUnits[i];
		if (su.GetOptionHash() == h) {
#ifdef NEX_DEBUG
			if (su.VerifyId(id))
				return i;
			else {
				// hash probably has conflict
				NEX_THROW_FatalError(EXCEPT_ASSERTION_FAILED);
			}
#endif
		}
	}
	return int32(-1);
}

int32 EffectAsset::_CreateUnit(const String& id, hash_t h, const ShaderOptions& options) {
	shaderUnits.emplace_back(h);
	ShaderUnit& unit = shaderUnits.back();

#ifdef NEX_DEBUG
	unit.SetId(id);
#endif

	const ShaderData* data = _GetShaderData();
	if (data == nullptr)
		return -1;
	if (!unit.Compile(*data, options)) {
		shaderUnits.pop_back();
		Error("Failed to compile shader!");
		return -1;
	}
		
	return (int32)shaderUnits.size() - 1;
}

} /* namespace nextar */
