/*
 * ConstBufferHLSL.cpp
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */
#include <ConstBufferTranslator.h>
#include <ConstBufferHLSL.h>

namespace ShaderCompiler {

ConstBufferHLSL::ConstBufferHLSL() {
}

ConstBufferHLSL::~ConstBufferHLSL() {
}

void ConstBufferHLSL::BeginBuffer(const String& name) {
	transientBuffer.clear();
	transientBuffer += "cbuffer " + name + " {\n";
}

void ConstBufferHLSL::AddParam(ParamDataType dataType, const String& name,
		uint32 arrayCount) {
	switch(dataType) {
	case PDT_BOOL:
		transientBuffer += "\t bool ";
		break;
	case PDT_UINT:
		transientBuffer += "\t uint ";
		break;
	case PDT_INT:
		transientBuffer += "\t int ";
		break;
	case PDT_FLOAT:
		transientBuffer += "\t float ";
		break;
	case PDT_VEC2:
		transientBuffer += "\t float2 ";
		break;
	case PDT_VEC3:
		transientBuffer += "\t float3 ";
		break;
	case PDT_VEC4:
		transientBuffer += "\t float4 ";
		break;
	case PDT_IVEC2:
		transientBuffer += "\t int2 ";
		break;
	case PDT_IVEC3:
		transientBuffer += "\t int3 ";
		break;
	case PDT_IVEC4:
		transientBuffer += "\t int4 ";
		break;
	case PDT_MAT4x4:
		transientBuffer += "\t float4x4 ";
		break;
	case PDT_MAT3x4:
		transientBuffer += "\t float3x4 ";
		break;
	}

	transientBuffer += name;
	if (arrayCount > 1)
		transientBuffer += "[" + Convert::ToString(arrayCount) + "]";
	transientBuffer += ";\n";
}

void ConstBufferHLSL::EndBuffer(ShaderScript* _script) {
	transientBuffer += "}\n";
	_script->AddRegion("cbuffer",  RenderManager::ShaderLanguage::SPP_HLSL,
			std::move(transientBuffer));
}

} /* namespace ShaderCompiler */
