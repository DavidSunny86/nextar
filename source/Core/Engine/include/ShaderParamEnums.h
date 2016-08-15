/*
 * ShaderParamEnums.h
 *
 *  Created on: 24-Jul-2014
 *      Author: obhi
 */

#ifndef SHADERPARAMENUMS_H_
#define SHADERPARAMENUMS_H_

#include <NexBase.h>

namespace nextar {

enum ParamDataType
	: uint16 {
	PDT_UNKNOWN,
	PDT_BOOL,
	PDT_UINT,
	PDT_INT,
	PDT_FLOAT,
	PDT_TEXTURE,
	PDT_VEC2,
	PDT_VEC3,
	PDT_VEC4,
	PDT_IVEC2,
	PDT_IVEC3,
	PDT_IVEC4,
	PDT_MAT4x4,
	PDT_MAT3x4,
	PDT_STRUCT,
	PDT_COUNT,
};

enum ParamDataBaseType : uint8 {
	BASE_UNKNOWN,
	BASE_BYTE,
	BASE_INT32,
	BASE_INT64,
	BASE_TEXTURE,
	BASE_FLOAT,
};

enum AutoParamName
	: uint16 {
	AUTO_INVALID_PARAM,
	// Inverse projection
	AUTO_INV_PROJECTION,
	// Model view projection and model view matrix
	AUTO_OBJECT_TRANSFORM,
	// Omni Light properties
	AUTO_OMNI_LIGHT_PROPERTIES,
	// Depth map
	AUTO_DEPTH_MAP,
	// Normal map
	AUTO_NORMAL_MAP,
	// Albedo  map
	AUTO_ALBEDO_MAP,
	// Specular and gloss map
	AUTO_SPECULAR_AND_GLOSS_MAP,
	// Model view projection matrix
	AUTO_MODEL_VIEW_PROJECTION,
	// Diffuse color
	AUTO_DIFFUSE_COLOR,
	// Constant scale factor
	AUTO_CONSTANT_SCALE_FACTOR,
	// Sky light
	AUTO_SKY_LIGHT,
	// Custom generic parameter, could be texture or constant
	// accessed from property buffer.
	AUTO_CUSTOM_CONSTANT,

	// Count
	AUTO_COUNT,
};

enum ParameterContext
	: uint8 {
	CTX_UNKNOWN = 255,
	// changes per frame: time
	// Type: shared
	CTX_FRAME = 0,
	// changes per view: view matrix
	// Type: shared
	CTX_VIEW,
	// Per pass change
	CTX_PASS,
	// changes per material instance: diffuseColor
	// Type: local to material
	CTX_MATERIAL,
	// changes per object instance: worldMatrix
	// Type: local to object/stored in shader
	CTX_OBJECT,
	CTX_COUNT
};


}



#endif /* SHADERPARAMENUMS_H_ */
