/*
 * ConstBufferGLSL.cpp
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */
#include <ConstBufferTranslator.h>
#include <ConstBufferGLSL.h>

namespace ShaderCompiler {

ConstBufferGLSL::ConstBufferGLSL() {
}

ConstBufferGLSL::~ConstBufferGLSL() {
}

void ConstBufferGLSL::BeginBuffer(const String& name) {
	transientBuffer.clear();
	transientBuffer += "uniform " + name + " {\n";
}

void ConstBufferGLSL::AddParam(ParamDataType dataType, const String& name,
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
		transientBuffer += "\t vec2 ";
		break;
	case PDT_VEC3:
		transientBuffer += "\t vec3 ";
		break;
	case PDT_VEC4:
		transientBuffer += "\t vec4 ";
		break;
	case PDT_IVEC2:
		transientBuffer += "\t ivec2 ";
		break;
	case PDT_IVEC3:
		transientBuffer += "\t ivec3 ";
		break;
	case PDT_IVEC4:
		transientBuffer += "\t ivec4 ";
		break;
	case PDT_MAT4x4:
		transientBuffer += "\t mat4 ";
		break;
	case PDT_MAT3x4:
		transientBuffer += "\t mat3x4 ";
		break;
	}

	transientBuffer += name;
	if (arrayCount > 1)
		transientBuffer += "[" + Convert::ToString(arrayCount) + "]";
	transientBuffer += ";\n";
}

void ConstBufferGLSL::EndBuffer(ShaderScript* _script) {
	transientBuffer += "}\n";
	_script->AddRegion("cbuffer",  RenderManager::ShaderLanguage::SPP_GLSL,
			std::move(transientBuffer));
}

} /* namespace ShaderCompiler */
