/*
 * GLShader.h
 *
 *  Created on: 02-Oct-2013
 *      Author: obhi
 */

#ifndef GLSHADER_H_
#define GLSHADER_H_

#include <GpuProgram.h>

namespace RenderOpenGL {

	/**
	 * @shared Shared across contexts from same driver object.
	 * */
	class GpuProgramGL: public nextar::GpuProgram {
		NEX_LOG_HELPER(GpuProgramGL);
	public:
		GpuProgramGL(GpuProgram* parentProgram);
		virtual ~GpuProgramGL();

		inline GLuint GetShaderObject() {
			return iGlShader;
		}

		virtual bool Compile(nextar::RenderContext*, const char* src, const nextar::StringUtils::WordList& macroList);
		virtual void Uncompile(nextar::RenderContext*);

	protected:
		GpuProgram* parentProgram;
		GLuint iGlShader;
	};

} /* namespace RenderOpenGL */
#endif /* GLSHADER_H_ */
