/*
 * GLShader.cpp
 *
 *  Created on: 02-Oct-2013
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <GpuProgramGL.h>
#include <RenderContext_Base_GL.h>

namespace RenderOpenGL {


GLenum GpuProgramGL::stagesMap[Pass::ProgramStage::STAGE_COUNT] = {
	GL_VERTEX_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_GEOMETRY_SHADER,
	GL_FRAGMENT_SHADER,
	GL_COMPUTE_SHADER
};

GpuProgramGL::GpuProgramGL() :
		iGlShader(0) {
}

GpuProgramGL::~GpuProgramGL() {
}

void GpuProgramGL::GetMacroList(
		const StringUtils::WordList& from,
		String& macroStr) {

	String value;
	StringPair valPair;
	ConstMultiStringHelper::Iterator it =  ConstMultiStringHelper::It(from);

	while (it.HasNext(value)) {
		valPair = StringUtils::Split(value);
		GetMacro(macroStr, valPair.first, valPair.second);
	}

	macroStr += "\n";
}

bool GpuProgramGL::Compile(Pass::ProgramStage::Type shaderType, RenderContext* ctx,
		const char* src,
		const String& flowIndicator,
		const nextar::StringUtils::WordList& macroList) {
	/* get program source */

	String macroStr;

	BaseRenderManager* baseRenderManager =
			static_cast<BaseRenderManager*>(RenderManager::InstancePtr());
	String stageName = Pass::ProgramStage::GetName(shaderType);
	stageName += "Stage";

	GetMacro(macroStr, stageName, "1");
	GetMacro(macroStr, flowIndicator, "1");
	GetMacroList(baseRenderManager->GetShaderOptions(shaderType), macroStr);
	GetMacroList(macroList, macroStr);
	/* todo append global options */
	iGlShader = static_cast<RenderContext_Base_GL*>(ctx)->CreateShader(stagesMap[shaderType],
			macroStr.c_str(), src);
	return (iGlShader != 0);
}

void GpuProgramGL::Destroy(RenderContext* ctx) {
	if (iGlShader)
		static_cast<RenderContext_Base_GL*>(ctx)->DestroyShader(iGlShader);
}

} /* namespace RenderOpenGL */
