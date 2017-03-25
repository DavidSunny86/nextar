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

	if (!m._reserved) {
		m._reserved = RenderManager::Instance().AllocMaterialRenderInfo();
		m.flags |= Material::RENDER_INFO_PER_PASS;
	}
	
	RenderInfo_Material* info = static_cast<RenderInfo_Material*>(m._reserved);
	uint32 count = renderSys->GetPassCount();

	for (uint32 i = 0; i < count; ++i) {
		RenderPass* pass = renderSys->GetPass(i);
		RenderPass::Info info = pass->GetPassInfo();

		ShaderOptions newOptions( info._options ? *info._options : StringUtils::Null );
		newOptions.Append(shaderOptions);
		newOptions.ToString(strOptions);
		hash_t h = StringUtils::Hash(strOptions);
		int32 unit = _FindUnit(strOptions, h);
		if (unit < 0) {
			unit = _CreateUnit(strOptions, h, newOptions, info);
		}
		info[i].shaderUnit = unit;
	}
	// combine options
	// for each render pass
	//   combine options
	//   find hash, if not available
	//     generate shader
	//   use shader to fill render info

}

void EffectAsset::AsyncAcquireData() {
}

void EffectAsset::AsyncReleaseData() {
}

int32 EffectAsset::_CreateUnit(const String& id, hash_t h, const ShaderOptions& options) {
	shaderUnits.emplace_back(h);
	ShaderUnit& unit = shaderUnits.back();

	const ShaderData& data = _GetShaderData();
	unit.Compile(data, options);

	return (int32)shaderUnits.size() - 1;
}

} /* namespace nextar */
