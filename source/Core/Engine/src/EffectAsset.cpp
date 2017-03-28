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


void EffectAsset::ReloadRequest::SetProgramSource(
	PipelineStage::Name stage, String&& src) {
	_data.programSources[stage] = std::move(src);
}

void EffectAsset::ReloadRequest::SetSemanticMap(
		VarToAutoParamMap&& m) {
	_data.autoNames = std::move(m);
}

void EffectAsset::ReloadRequest::AddSamplerUnit(TextureUnitParams& unit,
	StringUtils::WordList&& boundUnitNames) {
	_data.textureStates.push_back(SamplerDesc());
	SamplerDesc& sd = _data.textureStates.back();
	sd.texUnitParams = unit;
	sd.unitsBound = std::move(boundUnitNames);
}

void EffectAsset::ReloadRequest::AddAutoNameMapping(
		const String& varName, AutoParamName name) {
	_data.autoNames[StringUtils::Hash(varName)] = name;
}

EffectAsset::StreamRequest::StreamRequest(EffectAsset* owner) : 
ReloadRequest(owner),
renderQueueFlags(0) {
}

EffectAsset::StreamRequest::~StreamRequest() {
}

void EffectAsset::StreamRequest::SetRasterState(
		const RasterState& state) {
}

void EffectAsset::StreamRequest::SetBlendState(
		const BlendState& state) {
}

void EffectAsset::StreamRequest::SetDepthStencilState(
		const DepthStencilState& state) {
}

void EffectAsset::StreamRequest::SetRenderQueueFlags(uint32 flags) {
}


//****************************************
//            EffectAsset
//****************************************
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
	
	RenderInfo_Material* rinfo = static_cast<RenderInfo_Material*>(m._reserved_p);
	uint32 count = renderSys->GetPassCount();

	for (uint32 i = 0; i < count; ++i) {
		RenderPass* pass = renderSys->GetPass(i);
		RenderPass::Info info = pass->GetPassInfo();
		const ShaderData& data = _GetShaderData();

		if (_IsCompileAllowed(data, info)) {
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
	const StringUtils::WordList & renderPassOptions, Material & m) {
}

void EffectAsset::AsyncAcquireData() {
}

void EffectAsset::AsyncReleaseData() {
}

bool EffectAsset::_IsCompileAllowed(const ShaderData & data, const RenderPass::Info & info) {
	if (info.flags & RenderPass::PASS_OVERRIDES_MATERIAL)
		return false;
	if (std::find(data.tags.begin(), data.tags.end(), info.tag) != data.tags.end())
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

EffectAsset::ShaderData& EffectAsset::_GetShaderData() {
	// TODO: insert return statement here
}

int32 EffectAsset::_FindUnit(const String & id, hash_t h) {
	return int32();
}

int32 EffectAsset::_CreateUnit(const String& id, hash_t h, const ShaderOptions& options) {
	shaderUnits.emplace_back(h);
	ShaderUnit& unit = shaderUnits.back();

	const ShaderData& data = _GetShaderData();

	if (!unit.Compile(data, options)) {
		shaderUnits.pop_back();
		Error("Failed to compile shader!");
		return -1;
	}
		
	return (int32)shaderUnits.size() - 1;
}

} /* namespace nextar */
