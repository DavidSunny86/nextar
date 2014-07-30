/*
 * MaterialFormatv10.h
 *
 *  Created on: 28-Jul-2014
 *      Author: obhi
 */

#ifndef MATERIALFORMATV10_H_
#define MATERIALFORMATV10_H_

namespace nextar {

enum MaterialFormatChunkHeaders : uint16 {
	MATERIAL_HEADER = 0x3a7e,
	MATERIAL_PARAMETERS = 0x3196,
};

struct MaterialHeader {
	VersionID version;
	ShaderAsset::ID shader;
	URL shaderLocation;
	String options;
	uint8 layerMask;
};

}





#endif /* MATERIALFORMATV10_H_ */
