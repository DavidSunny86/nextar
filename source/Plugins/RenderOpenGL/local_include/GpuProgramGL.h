/*
 * GLShader.h
 *
 *  Created on: 02-Oct-2013
 *      Author: obhi
 */

#ifndef GLSHADER_H_
#define GLSHADER_H_

#include <RenderOpenGL.h>

namespace RenderOpenGL {

/**
 * @shared Shared across contexts from same driver object.
 * */
class GpuProgramGL {
	NEX_LOG_HELPER(GpuProgramGL)
	;
public:
	GpuProgramGL();
	virtual ~GpuProgramGL();

	inline GLuint GetShaderObject() {
		return iGlShader;
	}

	bool Compile(GLenum shaderType, nextar::RenderContext*, const char* src,
			const nextar::StringUtils::WordList& macroList);
	void Destroy(RenderContext* rc);

protected:
	GLuint iGlShader;
};

} /* namespace RenderOpenGL */
#endif /* GLSHADER_H_ */
