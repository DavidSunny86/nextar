/*
 * ShaderParam.h
 *
 *  Created on: 06-Oct-2013
 *      Author: obhi
 */

#ifndef SHADERPARAM_H_
#define SHADERPARAM_H_

#include "NexTypes.h"
#include "TextureUnitState.h"

namespace nextar {

	enum class ParamDataType {
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
		PDT_COUNT,
	};

	enum class AutoParamName : uint16 {
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
		// Custom indicates a custom parameter
		AUTO_CUSTOM
	};

	enum class UpdateFrequency {
		// changes per frame: view matrix
		PER_FRAME = 1 << 0,
		// changes per material instance: diffuseColor
		PER_MATERIAL = 1 << 1,
		// changes per object instance: worldMatrix
		PER_OBJECT_INSTANCE = 1 << 2,
		// changes per shader pass
		PER_PASS = 1 << 3,
	};

} // namespace nextar
#endif // SHADERPARAM_H_
