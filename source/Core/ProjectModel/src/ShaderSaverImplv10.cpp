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

namespace nextar {

ShaderSaverImplv10::ShaderSaverImplv10() {
}

ShaderSaverImplv10::~ShaderSaverImplv10() {
}

void ShaderSaverImplv10::SavePass(const ShaderTemplate::PassUnit& pu,
		ChunkOutputStream& stream) {
	OutputSerializer& ser = stream.BeginChunk(SHADER_PASS_BLOCK);
	ser << pu.name;

	// blend state
	ser << (uint8)PASS_BLEND_STATE
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
	ser << (uint8)PASS_DEPTH_STATE
		<< pu.depthStencilState.depthTest
		<< pu.depthStencilState.depthWrite
		<< pu.depthStencilState.depthCompareFunc
		<< pu.depthStencilState.stencilTest;
	if (pu.depthStencilState.stencilTest) {
		StencilFaceOp* op[2] = { &pu.depthStencilState.front, &pu.depthStencilState.back };
		for(uint32 i = 0; i < 2; ++i) {
			ser << (uint8)op[i]->stencilMask
				<< (uint8)op[i]->stencilFunc
				<< (uint8)op[i]->stencilFail
				<< (uint8)op[i]->stencilPass
				<< (uint8)op[i]->depthPass;
		}
	}

	// raster state
	ser << (uint8)PASS_RASTER_STATE
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
	ser << (uint8)PASS_TEXTURE_STATE;
	auto& tuMap = pu.textureUnitStates;
	for(auto& e : tuMap) {
		ser << e.first
			<< e.second.defaultTexturePath
			<< (uint8)e.second.params.minFilter
			<< (uint8)e.second.params.magFilter
			<< (uint8)e.second.params.uAddress
			<< (uint8)e.second.params.vAddress
			<< (uint8)e.second.params.wAddress
			<< (uint8)e.second.params.comparisonFunc
			<< (uint8)e.second.params.context
			<< e.second.params.unitType
			<< e.second.params.flags
			<< e.second.params.maxAnisotropy
			<< e.second.params.lodBias
			<< e.second.params.minLod
			<< e.second.params.maxLod
			<< e.second.params.borderColor;
	}

	auto& srcMap = pu.sourceMap;
	for(auto& m : srcMap) {
		OutputSerializer& srcChunk = stream.BeginChunk(SHADER_SOURCE);
		srcChunk << m.first
			<< m.second;
		stream.EndChunk();
	}
	stream.EndChunk();
}

void ShaderSaverImplv10::Save(OutputStreamPtr& out, AssetSaver& saver) {

	ShaderTemplate* shader = static_cast<ShaderTemplate*>(
			saver.GetRequestPtr()->GetStreamedObject());
	ChunkOutputStream stream(out);

	ShaderHeader header;
	header.numPasses = (uint8)shader->GetPassCount();
	header.numUnits = (uint16)shader->GetShaderCount();

	OutputSerializer& ser = stream.BeginChunk(SHADER_HEADER);
	ser << header.numPasses << header.numUnits;
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
}

} /* namespace nextar */
