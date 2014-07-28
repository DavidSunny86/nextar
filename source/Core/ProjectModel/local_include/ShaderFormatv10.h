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
	SHADER_SOURCE = 0x5244,
	SHADER_UNIT = 0x5134,
};

enum ShaderFormatKeys : uint16 {
	PASS_BLEND_STATE = 0xb19d,
	PASS_DEPTH_STATE = 0xde97,
	PASS_RASTER_STATE = 0x6a57,
	PASS_TEXTURE_STATE = 0x7e87,
};

struct ShaderHeader {
	uint16 numUnits;
	uint8 numPasses;
};

}



#endif /* SHADERFORMATV10_H_ */
