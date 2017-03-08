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

	bool Compile(Pass::ProgramStage::Type shaderType, nextar::RenderContext*, const char* src,
			const String& flowIndicator,
			const nextar::StringUtils::WordList& macroList);
	void Destroy(RenderContext* rc);

protected:

	static inline void GetMacro(String& oAppend, const String& name, const String& value) {
		if (name.length() > 0) {
			oAppend += "#define ";
			oAppend += name;
			oAppend += " ";
			oAppend += value;
			oAppend += "\n";
		}
	}

	void GetMacroList(const StringUtils::WordList& from, String& store);

	static GLenum stagesMap[Pass::ProgramStage::STAGE_COUNT];
	GLuint iGlShader;
};

} /* namespace RenderOpenGL */
#endif /* GLSHADER_H_ */
