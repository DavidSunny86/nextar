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

GpuProgramGL::GpuProgramGL() :
		iGlShader(0) {
}

GpuProgramGL::~GpuProgramGL() {
}

bool GpuProgramGL::Compile(GLenum shaderType, RenderContext* ctx,
		const char* src, const nextar::StringUtils::WordList& macroList) {
	/* get program source */
	String macroStr, macro;
	ConstMultiStringHelper::Iterator it =  ConstMultiStringHelper::It(macroList);
	
	while (it.HasNext(macro)) {
		macroStr += "#define ";
		macroStr += macro;
		macroStr += "\n";
	}

	macroStr += "\n";
	/* todo append global options */
	iGlShader = static_cast<RenderContext_Base_GL*>(ctx)->CreateShader(shaderType,
			macroStr.c_str(), src);
	return (iGlShader != 0);
}

void GpuProgramGL::Destroy(RenderContext* ctx) {
	if (iGlShader)
		static_cast<RenderContext_Base_GL*>(ctx)->DestroyShader(iGlShader);
}

} /* namespace RenderOpenGL */
