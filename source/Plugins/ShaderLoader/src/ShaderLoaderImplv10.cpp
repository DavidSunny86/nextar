/*
 * ShaderLoaderImplv10.cpp
 *
 *  Created on: 29-Jul-2014
 *      Author: obhi
 */

#include <ShaderLoaderImplv10.h>
#include <RenderManager.h>

namespace ShaderLoader {

enum ShaderFormatChunkHeaders : uint16 {
	SHADER_HEADER = 0x5231,
	SHADER_PASS_BLOCK = 0x5633,
	/* @shader_lang */
	SHADER_SOURCE_GLSL = 0x5244,
	SHADER_SOURCE_HLSL = 0x5114,
	SHADER_UNIT = 0x5134,
};

enum ShaderFormatKeys : uint16 {
	PASS_BLEND_STATE = 0xb19d,
	PASS_DEPTH_STATE = 0xde97,
	PASS_RASTER_STATE = 0x6a57,
	PASS_TEXTURE_STATE = 0x7e87,
	PASS_SEMANTIC_MAP = 0x1fde,
};

struct ShaderHeader {
	VersionID version;
	uint16 numUnits;
	uint8 numPasses;
	uint32 renderFlags;
};

ShaderLoaderImplv1_0::ShaderLoaderImplv1_0() : language(RenderManager::SPP_UNKNOWN) {
}

ShaderLoaderImplv1_0::~ShaderLoaderImplv1_0() {
}

bool ShaderLoaderImplv1_0::ReadPass(ShaderAsset::StreamRequest* request,
		ChunkInputStream& ser) {
	ShaderAsset* shader = static_cast<ShaderAsset*>(
					request->GetStreamedObject());
	StringID name;
	uint16 header;
	ser >> name;
	request->AddPass(name);

	ser >> header;
	if (header != PASS_BLEND_STATE) {
		Error("Shader does not have a valid blend state: " + request->GetName());
		return false;
	}
	BlendState blendState;
	ser >> blendState.enabled;
	if (blendState.enabled) {
		ser >> blendState.alphaToCoverage
			>> blendState.numRenderTargets;
		for(uint16 i = 0; i < blendState.numRenderTargets; ++i) {
			auto& blendOp = blendState.blendOp[i];
			ser >> blendOp.enabled;
			if (blendOp.enabled) {
				uint8 mask, colOp, alphaOp, srcCol, srcAlpha, destCol, destAlpha;
				ser >> mask
					>> colOp
					>> alphaOp
					>> srcCol
					>> srcAlpha
					>> destCol
					>> destAlpha;
				blendOp.mask = (ColorMask)mask;
				blendOp.colOp = (BlendOp)colOp;
				blendOp.alphaOp = (BlendOp)alphaOp;
				blendOp.srcCol = (BlendDataSource)srcCol;
				blendOp.srcAlpha = (BlendDataSource)srcAlpha;
				blendOp.destCol = (BlendDataSource)destCol;
				blendOp.destAlpha = (BlendDataSource)destAlpha;
			}
		}
	}
	request->SetBlendState(blendState);
	ser >> header;
	if (header != PASS_DEPTH_STATE) {
		Error("Shader does not have a valid depth state: " + request->GetName());
		return false;
	}
	DepthStencilState depthStencilState;
	uint8 depthCompareFunc;
	ser >> depthStencilState.depthTest
		>> depthStencilState.depthWrite
		>> depthCompareFunc
		>> depthStencilState.stencilTest;
	depthStencilState.depthCompareFunc = (DepthStencilCompare)depthCompareFunc;
	if(depthStencilState.stencilTest) {
		StencilFaceOp* op[2] = { &depthStencilState.front, &depthStencilState.back };
		uint8 stencilFunc, stencilFail, stencilPass, depthPass;
		uint32 stencilMask;
		int32 stencilRef;
		for(uint32 i = 0; i < 2; ++i) {
			ser >> stencilMask
				>> stencilRef
				>> stencilFunc
				>> stencilFail
				>> stencilPass
				>> depthPass;
			op[i]->stencilMask = stencilMask;
			op[i]->stencilRef = stencilRef;
			op[i]->stencilFunc = (DepthStencilCompare)stencilFunc;
			op[i]->stencilFail = (StencilOp)stencilFail;
			op[i]->stencilPass = (StencilOp)stencilPass;
			op[i]->depthPass = (StencilOp)depthPass;
		}
	}
	request->SetDepthStencilState(depthStencilState);

	ser >> header;
	if (header != PASS_RASTER_STATE) {
		Error("Shader does not have a valid raster state: " + request->GetName());
		return false;
	}
	RasterState rasterState;
	uint8 fill, cull;
	ser >> rasterState.trianglesAreClockwise
			>> rasterState.depthClip
			>> rasterState.usingScissors
			>> rasterState.usingMultisample
			>> rasterState.usingLineAa
			>> fill
			>> cull
			>> rasterState.constantDepthBias
			>> rasterState.slopeScaledDepthBias
			>> rasterState.depthBiasClamp;
	rasterState.fill = (FillMode)fill;
	rasterState.cull = (CullMode)cull;
	request->SetRasterState(rasterState);
	ser >> header;
	if (header != PASS_TEXTURE_STATE) {
		Error("Shader does not have a valid texture state: " + request->GetName());
		return false;
	}
	String unitName;
	uint32 numUnits;
	ser >> numUnits;
	for(uint32 i = 0; i < numUnits; ++i) {
		TextureUnitParams params;
		uint8 minFilter, magFilter, uAddress, vAddress, wAddress, comparisonFunc, context, unitType;
		String unitsBound;

		ser >> unitName
			>> minFilter
			>> magFilter
			>> uAddress
			>> vAddress
			>> wAddress
			>> comparisonFunc
			>> context
			>> unitType
			>> params.flags
			>> params.maxAnisotropy
			>> params.lodBias
			>> params.minLod
			>> params.maxLod
			>> params.borderColor
			>> unitsBound;
		params.minFilter = (TextureMinFilter)minFilter;
		params.magFilter = (TextureMagFilter)magFilter;
		params.uAddress = (TextureAddressMode)uAddress;
		params.vAddress = (TextureAddressMode)vAddress;
		params.wAddress = (TextureAddressMode)wAddress;
		params.comparisonFunc = (TextureComparisonMode)comparisonFunc;
		params.context = (ParameterContext)context;
		params.unitType = (TextureUnitType)unitType;
		request->AddSamplerUnit(params, unitsBound);
	}

	ser >> header;
	if (header != PASS_SEMANTIC_MAP) {
		Error("Shader does not have a valid semantic map: " + request->GetName());
		return false;
	}

	uint32 numEntries;
	ser >> numEntries;
	for (uint32 i = 0; i < numEntries; ++i) {
		String varName; uint16 autoVal;
		ser >> varName
			>> autoVal;
		request->AddAutoNameMapping(varName, (AutoParamName)autoVal);
	}

	std::array<ShaderFormatChunkHeaders, (uint32)RenderManager::SPP_COUNT> 	languageDumps;
	languageDumps[RenderManager::SPP_GLSL] = SHADER_SOURCE_GLSL;
	languageDumps[RenderManager::SPP_HLSL] = SHADER_SOURCE_HLSL;
	NEX_ASSERT ((uint32)language < RenderManager::SPP_COUNT);
	ChunkInputStream::Chunk shaderCode = ChunkInputStream::Invalid;
	ser.ReadChunk((uint16)languageDumps[language], shaderCode);
	if (ChunkInputStream::IsValid(shaderCode)) {
		uint32 numStages;
		ser >> numStages;
		for(uint32 i = 0; i < numStages; ++i) {
			uint16 stage;
			String source;
			ser >> stage >> source;
			request->SetProgramSource((Pass::ProgramStage)stage, std::move(source));
		}
	} else
		return false;

	return true;
}

void ShaderLoaderImplv1_0::Load(InputStreamPtr& stream,
		AssetLoader& loader) {

	if (language == RenderManager::SPP_UNKNOWN)
		language =
		RenderManager::Instance().GetProgramLanguage();

	ShaderAsset::StreamRequest* request =
			static_cast<ShaderAsset::StreamRequest*>(loader.GetRequestPtr());
	ShaderAsset* shader = static_cast<ShaderAsset*>(
				request->GetStreamedObject());
	ShaderHeader header;

	ChunkInputStream ser(stream);
	InputSerializer::Chunk chunk = ChunkInputStream::Invalid;
	ser.ReadChunk(SHADER_HEADER, chunk);
	if (InputSerializer::IsValid(chunk)) {
		ser >> header.version;
		if (!TestVersion(NEX_MAKE_VERSION(1,0,0), header.version)) {
			Error("Shader is of higher version: " + request->GetName());
			request->SetCompleted(false);
			return;
		}

		ser >> header.numPasses >> header.numUnits >> header.renderFlags;
	} else {
		Error("Shader does not have a valid header: " + request->GetName());
		request->SetCompleted(false);
		return;
	}
	request->SetRenderQueueFlags(header.renderFlags);

	bool foundUnit = false;
	StringID name;
	do {
		ser >> chunk;
		if (InputSerializer::IsValid(chunk)) {
			switch (chunk.first.first) {
			case SHADER_PASS_BLOCK:
				if (!ReadPass(request, ser)) {
					request->SetCompleted(false);
					return;
				}
				break;
			case SHADER_UNIT:
				ser >> name;
				if (name == shader->GetID()) {
					foundUnit = true;
					String compilationOptions;
					ser >> compilationOptions;
					request->SetCompilationOptions(compilationOptions);
				}
				break;
			}
		} else {
			Error(String("File is corrupt: ") + request->GetName());
			request->SetCompleted(false);
			return;
		}
	} while (!ser.IsEndOfStream());

	if (!foundUnit) {
		Error(String("Could not find shader unit in file: ") + request->GetName());
		request->SetCompleted(false);
		return;
	}

	request->SetCompleted(true);
}

} /* namespace ShaderLoader */
