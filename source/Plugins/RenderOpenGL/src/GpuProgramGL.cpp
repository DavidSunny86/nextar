/*
 * GLShader.cpp
 *
 *  Created on: 02-Oct-2013
 *      Author: obhi
 */
#include "RenderOpenGL.h"
#include "GpuProgramGL.h"

namespace RenderOpenGL {

	GpuProgramGL::GpuProgramGL(GpuProgram* _parentProgram) :
		parentProgram(_parentProgram), iGlShader(0) {
	}

	GpuProgramGL::~GpuProgramGL() {
	}

	bool GpuProgramGL::Compile(RenderContext* ctx, const char* src,const nextar::StringUtils::WordList& macroList) {
		GLenum shaderType = 0;
		switch(parentProgram->GetType()) {
		case GpuProgram::TYPE_VERTEX:
			shaderType = GL_VERTEX_SHADER;
			break;
		case GpuProgram::TYPE_HULL:
			shaderType = GL_TESS_CONTROL_SHADER;
			break;
		case GpuProgram::TYPE_DOMAIN:
			shaderType = GL_TESS_EVALUATION_SHADER;
			break;
		case GpuProgram::TYPE_GEOMETRY:
			shaderType = GL_GEOMETRY_SHADER;
			break;
		case GpuProgram::TYPE_FRAGMENT:
			shaderType = GL_FRAGMENT_SHADER;
			break;
		case GpuProgram::TYPE_COMPUTE:
			shaderType = GL_COMPUTE_SHADER;
			break;
		}
		/* get program source */
		String macroStr, macro;
		StringUtils::TokenIterator it = 0;

		while ((it=StringUtils::NextWord(macroList, macro, it)) != String::npos) {
			macroStr += "#define ";
			macroStr += macro;
			macroStr += "\n";
		}

		macroStr += "\n";
		/* todo append global options */
		const char* sourceStr = parentProgram->GetSource();

		iGlShader = static_cast<RenderContextGL*>(ctx)->CreateShader(shaderType, macroStr.c_str(),
				sourceStr);
		return (iGlShader != 0);
	}

	void GpuProgramGL::Uncompile(RenderContext* ctx) {
		static_cast<RenderContextGL*>(ctx)->DestroyShader(iGlShader);
	}

} /* namespace RenderOpenGL */
