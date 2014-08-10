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
};

struct ShaderHeader {
	VersionID version;
	uint16 numUnits;
	uint8 numPasses;
};

ShaderLoaderImplv1_0::ShaderLoaderImplv1_0() : language(RenderManager::SPP_UNKNOWN) {
}

ShaderLoaderImplv1_0::~ShaderLoaderImplv1_0() {
}

void ShaderLoaderImplv1_0::ReadPass(ShaderAsset::StreamRequest* request,
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
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
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
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
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
		uint8 stencilMask, stencilFunc, stencilFail, stencilPass, depthPass;
		for(uint32 i = 0; i < 2; ++i) {
			ser >> stencilMask
				>> stencilFunc
				>> stencilFail
				>> stencilPass
				>> depthPass;
			op[i]->stencilMask = (ColorMask)stencilMask;
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
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
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
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}
	String unitName;
	uint32 numUnits;
	ser >> numUnits;
	for(uint32 i = 0; i < numUnits; ++i) {
		TextureUnitParams params;
		uint8 minFilter, magFilter, uAddress, vAddress, wAddress, comparisonFunc, context;
		bool defaultTexture;
		ser >> unitName
			>> minFilter
			>> magFilter
			>> uAddress
			>> vAddress
			>> wAddress
			>> comparisonFunc
			>> context
			>> params.unitType
			>> params.flags
			>> params.maxAnisotropy
			>> params.lodBias
			>> params.minLod
			>> params.maxLod
			>> params.borderColor
			>> defaultTexture;
		params.minFilter = (TextureMinFilter)minFilter;
		params.magFilter = (TextureMagFilter)magFilter;
		params.uAddress = (TextureAddressMode)uAddress;
		params.vAddress = (TextureAddressMode)vAddress;
		params.wAddress = (TextureAddressMode)wAddress;
		params.comparisonFunc = (TextureComparisonMode)comparisonFunc;
		params.context = (ParameterContext)context;
		TextureAssetPtr texture;
		if (defaultTexture) {
			URL locator;
			TextureAsset::ID id;
			ser >> id >> locator;
			texture = TextureAsset::Traits::Instance(id, locator);
		}
		request->AddTextureUnit(unitName, params, texture);
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
	}
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
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}

		ser >> header.numPasses >> header.numUnits;
	} else {
		Error("Shader does not have a valid header: " + request->GetName());
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}

	bool foundUnit = false;
	StringID name;
	do {
		ser >> chunk;
		if (InputSerializer::IsValid(chunk)) {
			switch (chunk.first.first) {
			case SHADER_PASS_BLOCK:
				ReadPass(request, ser);
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
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}
	} while (!ser.IsEndOfStream());

	if (!foundUnit) {
		Error(String("Could not find shader unit in file: ") + request->GetName());
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}
}

} /* namespace ShaderLoader */
