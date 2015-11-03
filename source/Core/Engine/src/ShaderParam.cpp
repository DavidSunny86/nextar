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

ParamDataType ShaderParameter::MapParamType(const String& typeName) {
	typedef std::pair<const char*, ParamDataType> ParamTypeNamePair;
	static ParamTypeNamePair paramTypeNameTable[] = {
		ParamTypeNamePair("bool", ParamDataType::PDT_BOOL),
		ParamTypeNamePair("count", ParamDataType::PDT_COUNT),
		ParamTypeNamePair("float", ParamDataType::PDT_FLOAT),
		ParamTypeNamePair("int", ParamDataType::PDT_INT),
		ParamTypeNamePair("ivec2", ParamDataType::PDT_IVEC2),
		ParamTypeNamePair("ivec3", ParamDataType::PDT_IVEC3),
		ParamTypeNamePair("ivec4", ParamDataType::PDT_IVEC4),
		ParamTypeNamePair("mat3x4", ParamDataType::PDT_MAT3x4),
		ParamTypeNamePair("mat4x4", ParamDataType::PDT_MAT4x4),
		ParamTypeNamePair("struct", ParamDataType::PDT_STRUCT),
		ParamTypeNamePair("texture", ParamDataType::PDT_TEXTURE),
		ParamTypeNamePair("uint", ParamDataType::PDT_UINT),
		ParamTypeNamePair("unknown", ParamDataType::PDT_UNKNOWN),
		ParamTypeNamePair("vec2", ParamDataType::PDT_VEC2),
		ParamTypeNamePair("vec3", ParamDataType::PDT_VEC3),
		ParamTypeNamePair("vec4", ParamDataType::PDT_VEC4),
	};
	static const uint32 arraySize = sizeof(paramTypeNameTable) / sizeof(paramTypeNameTable[0]);
	ParamTypeNamePair* last = paramTypeNameTable + arraySize;
	ParamTypeNamePair* ptr = std::lower_bound(paramTypeNameTable, last, typeName,
		[](const ParamTypeNamePair& p1, const String& searchText) -> bool {
		return searchText.compare(p1.first) > 0;
	});


	if (ptr && ptr != last && !(typeName.compare(ptr->first) < 0)) {
		return ptr->second;
	}

	return ParamDataType::PDT_UNKNOWN;
}

ParamDataBaseType ShaderParameter::GetBaseType(ParamDataType type) {
	switch (type) {
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

