/*
 * ShaderSaverImplv10.cpp
 *
 *  Created on: 21-Jul-2014
 *      Author: obhi
 */
#include <NexProjectModel.h>
#include <ShaderSaverImplv10.h>
#include <Serializer.h>
#include <ShaderFormatv10.h>
#include <ShaderTemplate.h>

namespace nextar {

ShaderSaverImplv1_0 ShaderSaverImplv1_0::saver;

ShaderSaverImplv1_0::ShaderSaverImplv1_0() {
}

ShaderSaverImplv1_0::~ShaderSaverImplv1_0() {
}

void ShaderSaverImplv1_0::SavePass(const ShaderTemplate::PassUnit& pu,
		ChunkOutputStream& stream) {
	OutputSerializer& ser = stream.BeginChunk(SHADER_PASS_BLOCK);
	ser << pu.name;

	// blend state
	ser << (uint16)PASS_BLEND_STATE
		<<	pu.blendState.enabled;
	if (pu.blendState.enabled) {
		ser << pu.blendState.alphaToCoverage
		<< pu.blendState.numRenderTargets;
		for(uint16 i = 0; i < pu.blendState.numRenderTargets; ++i) {
			auto blendOp = pu.blendState.blendOp[i];
			ser << blendOp.enabled;
			if (blendOp.enabled) {
				ser << (uint8)blendOp.mask
					<< (uint8)blendOp.colOp
					<< (uint8)blendOp.alphaOp
					<< (uint8)blendOp.srcCol
					<< (uint8)blendOp.srcAlpha
					<< (uint8)blendOp.destCol
					<< (uint8)blendOp.destAlpha;
			}
		}
	}

	// depth stencil state
	ser << (uint16)PASS_DEPTH_STATE
		<< pu.depthStencilState.depthTest
		<< pu.depthStencilState.depthWrite
		<< (uint8)pu.depthStencilState.depthCompareFunc
		<< pu.depthStencilState.stencilTest;
	if (pu.depthStencilState.stencilTest) {
		const StencilFaceOp* op[2] = { &pu.depthStencilState.front, &pu.depthStencilState.back };
		for(uint32 i = 0; i < 2; ++i) {
			ser << (uint32)op[i]->stencilMask
				<< (int32)op[i]->stencilRef
				<< (uint8)op[i]->stencilFunc
				<< (uint8)op[i]->stencilFail
				<< (uint8)op[i]->stencilPass
				<< (uint8)op[i]->depthPass;
		}
	}

	// raster state
	ser << (uint16)PASS_RASTER_STATE
		<< pu.rasterState.trianglesAreClockwise
		<< pu.rasterState.depthClip
		<< pu.rasterState.usingScissors
		<< pu.rasterState.usingMultisample
		<< pu.rasterState.usingLineAa
		<< (uint8)pu.rasterState.fill
		<< (uint8)pu.rasterState.cull
		<< pu.rasterState.constantDepthBias
		<< pu.rasterState.slopeScaledDepthBias
		<< pu.rasterState.depthBiasClamp;

	// todo Texture unit states and source map
	ser << (uint16)PASS_TEXTURE_STATE;
	auto& tuMap = pu.textureUnitStates;
	uint32 numUnits = (uint32)tuMap.size();
	ser << numUnits;
	for(auto& e : tuMap) {
		ser << e.first
			<< (uint8)e.second.params.minFilter
			<< (uint8)e.second.params.magFilter
			<< (uint8)e.second.params.uAddress
			<< (uint8)e.second.params.vAddress
			<< (uint8)e.second.params.wAddress
			<< (uint8)e.second.params.comparisonFunc
			<< (uint8)e.second.params.context
			<< (uint8)e.second.params.unitType
			<< e.second.params.flags
			<< e.second.params.maxAnisotropy
			<< e.second.params.lodBias
			<< e.second.params.minLod
			<< e.second.params.maxLod
			<< e.second.params.borderColor
			<< e.second.unitsBound;
	}
	

	uint32 numEntries = (uint32)pu.semanticMap.size();
	ser << (uint16)PASS_SEMANTIC_MAP;
	ser << numEntries;
	for (auto& e : pu.semanticMap) {
		ser << e.first
			<< (uint16)e.second;
	}

	auto& srcMap = pu.sourceMap;
	std::array<ShaderFormatChunkHeaders, (uint32)RenderManager::SPP_COUNT> 	languageDumps;
	languageDumps[RenderManager::SPP_GLSL] = SHADER_SOURCE_GLSL;
	languageDumps[RenderManager::SPP_HLSL] = SHADER_SOURCE_HLSL;

	for(uint32 i = 0; i < languageDumps.size(); ++i) {
		auto range = srcMap.equal_range((RenderManager::ShaderLanguage)i);
		if (range.first != srcMap.end()) {
			OutputSerializer& ser = stream.BeginChunk((uint16)(languageDumps[i]));
			uint32 numStages = (uint32)std::distance(range.first, range.second);
			ser << numStages;
			for(auto it = range.first; it != range.second; ++it) {
				uint16 stage = (uint16)(*it).second.first;
				ser <<  stage << (*it).second.second;
			}
			stream.EndChunk();
		}
	}
	stream.EndChunk();
}

void ShaderSaverImplv1_0::Save(OutputStreamPtr& out, AssetSaver& saver) {

	ShaderTemplate* shader = static_cast<ShaderTemplate*>(
			saver.GetRequestPtr()->GetStreamedObject());
	ChunkOutputStream stream(out);

	ShaderHeader header;
	header.version = NEX_MAKE_VERSION(1, 0, 0);
	header.numPasses = (uint8)shader->GetPassCount();
	header.numUnits = (uint16)shader->GetShaderCount();
	header.renderFlags = shader->GetRenderFlags();
	OutputSerializer& ser = stream.BeginChunk(SHADER_HEADER);
	ser << header.version << header.numPasses << header.numUnits << header.renderFlags;
	stream.EndChunk();

	auto &passList = shader->GetPassList();
	for(auto &p : passList) {
		SavePass(p, stream);
	}

	// save individual shader units
	auto& shaderTable = shader->GetShaderTable();

	for(auto& s : shaderTable) {
		OutputSerializer& ser = stream.BeginChunk(SHADER_UNIT);
		ser << s.second.shaderObject->GetID()
			<< s.second.compilationOptions;
		stream.EndChunk();
	}

	saver.GetRequestPtr()->SetCompleted(true);
}

} /* namespace nextar */
