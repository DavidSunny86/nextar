/*
 * ShaderParam.cpp
 *
 *  Created on: 06-Oct-2013
 *      Author: obhi
 */

#include <ShaderParam.h>

namespace nextar {
	uint32 ConstantParameter::stride = 0;
	uint32 SamplerParameter::stride = 0;

	ParamDataBaseType ShaderParameter::GetBaseType(ParamDataType type) {
		switch(type) {
		case ParamDataType::PDT_BOOL:
			return ParamDataBaseType::BASE_BYTE;
		case ParamDataType::PDT_UINT:
		case ParamDataType::PDT_INT:
		case ParamDataType::PDT_IVEC2:
		case ParamDataType::PDT_IVEC3:
		case ParamDataType::PDT_IVEC4:
			return ParamDataBaseType::BASE_INT32;
		case ParamDataType::PDT_TEXTURE:
			// pointer??
			return ParamDataBaseType::BASE_TEXTURE;
		case ParamDataType::PDT_FLOAT:
		case ParamDataType::PDT_VEC2:
		case ParamDataType::PDT_VEC3:
		case ParamDataType::PDT_VEC4:
		case ParamDataType::PDT_MAT4x4:
		case ParamDataType::PDT_MAT3x4:
			return ParamDataBaseType::BASE_FLOAT;
		case ParamDataType::PDT_STRUCT:
		case ParamDataType::PDT_UNKNOWN:
		default:
			return ParamDataBaseType::BASE_UNKNOWN;
		}
	}
} /* namespace nextar */

