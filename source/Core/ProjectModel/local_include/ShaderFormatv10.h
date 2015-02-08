/*
 * ShaderFormatv10.h
 *
 *  Created on: 22-Jul-2014
 *      Author: obhi
 */

#ifndef SHADERFORMATV10_H_
#define SHADERFORMATV10_H_

namespace nextar {

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

}



#endif /* SHADERFORMATV10_H_ */
