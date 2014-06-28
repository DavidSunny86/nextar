/*
 * GLShader.cpp
 *
 *  Created on: 02-Oct-2013
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <GpuProgramGL.h>
#include <RenderContextGL.h>

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
	StringUtils::TokenIterator it = 0;

	while ((it = StringUtils::NextWord(macroList, macro, it)) != String::npos) {
		macroStr += "#define ";
		macroStr += macro;
		macroStr += "\n";
	}

	macroStr += "\n";
	/* todo append global options */
	iGlShader = static_cast<RenderContextGL*>(ctx)->CreateShader(shaderType,
			macroStr.c_str(), src);
	return (iGlShader != 0);
}

void GpuProgramGL::Destroy(RenderContext* ctx) {
	static_cast<RenderContextGL*>(ctx)->DestroyShader(iGlShader);
}

} /* namespace RenderOpenGL */
