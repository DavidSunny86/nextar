/*
 * TranslatorHLSL.cpp
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */
#include <LanguageTranslator.h>
#include <TranslatorHLSL.h>

namespace ShaderCompiler {

TranslatorHLSL::TranslatorHLSL() {
}

TranslatorHLSL::~TranslatorHLSL() {
}

void TranslatorHLSL::BeginBuffer(const String& name) {
	transientBuffer.clear();
	transientBuffer += "cbuffer " + name + " {\n";
}

void TranslatorHLSL::AddParam(ParamDataType dataType, const String& name,
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

void TranslatorHLSL::EndBuffer(ShaderScript* _script) {
	transientBuffer += "}\n";
	_script->AddRegion(_SS(REG_CBUFFER), RenderManager::ShaderLanguage::SPP_HLSL,
			std::move(transientBuffer));
}

void TranslatorHLSL::AddMacro(ShaderScript* script, const String& name) {
	String val = "#define ";
	val += name;
	val += "\n";
	script->AddRegion(_SS(REG_DEFINE), RenderManager::ShaderLanguage::SPP_HLSL,
		std::move(val));
}

void TranslatorHLSL::AddPredefs(Pass::ProgramStage stage, ShaderScript* script) {

}

} /* namespace ShaderCompiler */
