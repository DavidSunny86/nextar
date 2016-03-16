/*
 * TranslatorGLSL.cpp
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */
#include <LanguageTranslator.h>
#include <ScriptStrings.h>
#include <ShaderScriptContext.h>
#include <TranslatorGLSL.h>

namespace ShaderScript {

TranslatorGLSL::TranslatorGLSL() {
}

TranslatorGLSL::~TranslatorGLSL() {
}

void TranslatorGLSL::BeginBuffer(ShaderScriptContext* _script, const String& name) {
	String& transientBuffer = _script->_transientBuffer[GLSL_TRANSLATOR];
	transientBuffer.clear();
	transientBuffer += "layout(std140) uniform " + name + " {\n";
}

void TranslatorGLSL::AddParam(ShaderScriptContext* script,
		ParamDataType dataType, const String& name,
		uint32 arrayCount) {
	String& transientBuffer = script->_transientBuffer[GLSL_TRANSLATOR];
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

void TranslatorGLSL::EndBuffer(ShaderScriptContext* _script) {
	String& transientBuffer = _script->_transientBuffer[GLSL_TRANSLATOR];
	transientBuffer += "};\n";
	_script->AddRegion(_SS(REG_CBUFFER), RenderManager::ShaderLanguage::SPP_GLSL,
			std::move(transientBuffer));
}

void TranslatorGLSL::AddMacro(ShaderScriptContext* script, const String& name) {
	String val = "#define ";
	val += name;
	val += "\n";
	script->AddRegion("define", RenderManager::ShaderLanguage::SPP_GLSL,
		std::move(val));
}

void TranslatorGLSL::AddPredefs(ShaderScriptContext* script, Pass::ProgramStage stage) {
}

} /* namespace ShaderCompiler */
