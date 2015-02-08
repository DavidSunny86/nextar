/*
 * TranslatorGLSL.h
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERGLSL_H_
#define PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERGLSL_H_

#include <LanguageTranslator.h>

namespace ShaderCompiler {

class TranslatorGLSL :
		public AllocGeneral,
		public LanguageTranslatorIntf {
public:
	TranslatorGLSL();
	virtual ~TranslatorGLSL();

	virtual void BeginBuffer(const String& name);
	virtual void AddParam(ParamDataType dataType, const String& name, uint32 arrayCount);
	virtual void EndBuffer(ShaderScript* _script);

	virtual void AddMacro(ShaderScript* script, const String& name);
	virtual void AddPredefs(Pass::ProgramStage stage, ShaderScript* script);

protected:
	String transientBuffer;
};

} /* namespace ShaderCompiler */

#endif /* PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERGLSL_H_ */
