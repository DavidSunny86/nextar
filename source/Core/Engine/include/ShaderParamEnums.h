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

enum class AutoParamName
	: uint16 {
	// Time in seconds
	AUTO_TIME,
	// Elapsed time in seconds between two frames ( f(i) and f(i-1) )
	AUTO_ELAPSED_TIME,
	// Frame Number
	AUTO_FRAME_NUMBER,
	// Diffuse Color
	AUTO_DIFFUSE_COLOR,
	// Specualar power
	AUTO_SPECULAR_POWER,
	// World matrix.
	AUTO_WORLD,
	// Inverse world matrix, only 1 accepted
	AUTO_INV_WORLD,
	// View matrix.
	AUTO_VIEW,
	// Projection matrix.
	AUTO_PROJECTION,
	// Inverse projection
	AUTO_INV_PROJECTION,
	// World*View matrix.
	AUTO_WORLD_VIEW,
	// View*Projection matrix.
	AUTO_VIEW_PROJECTION,
	// World*View*Projection matrix.
	AUTO_WORLD_VIEW_PROJECTION,
	// Light position
	AUTO_LIGHT_POSITION,
	// Light direction
	AUTO_LIGHT_DIRECTION,
	// Light position
	AUTO_LIGHT_OBJ_SPACE_POSITION,
	// Light direction
	AUTO_LIGHT_OBJ_SPACE_DIRECTION,
	// Light color
	AUTO_LIGHT_SPECUALR_COLOR,
	// Ambient color
	AUTO_LIGHT_AMBIENT_COLOR,
	// Diffuse color
	AUTO_LIGHT_DIFFUSE_COLOR,
	// Atten range + coefficients color
	AUTO_LIGHT_ATTENUATION,
	// Light pov matrix used for shadow coordinates
	AUTO_LIGHT_POV_MATRIX,
	// Model view projection and model view matrix
	AUTO_OBJECT_TRANSFORM,
	// Omni Light properties
	AUTO_OMNI_LIGHT_PROPERTIES,
	// Depth map
	AUTO_DEPTH_MAP,
	// Normal map
	AUTO_NORMAL_MAP,
	// Albedo and gloss map
	AUTO_ALBEDO_AND_GLOSS,
	// Custom generic parameter, could be texture or constant
	// accessed from property buffer.
	AUTO_CUSTOM_CONSTANT,

	// Count
	AUTO_COUNT,
};

enum class ParameterContext
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
