/*
 * TranslatorGLSL.h
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERGLSL_H_
#define PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERGLSL_H_

#include <LanguageTranslator.h>

namespace ShaderScript {

class TranslatorGLSL :
		public AllocGeneral,
		public LanguageTranslatorIntf {
public:
	NEX_SINGLE_INSTANCE(TranslatorGLSL);
	TranslatorGLSL();
	virtual ~TranslatorGLSL();

	virtual void BeginBuffer(ShaderScriptContext* script, const String& name)  override;
	virtual void AddParam(ShaderScriptContext* script, ParamDataType dataType, const String& name, uint32 arrayCount)  override;
	virtual void EndBuffer(ShaderScriptContext* script)  override;

	virtual void AddMacro(ShaderScriptContext* script, const String& name)  override;
	virtual void AddPredefs(ShaderScriptContext* script, Pass::ProgramStage stage)  override;

protected:
};

} /* namespace ShaderCompiler */

#endif /* PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERGLSL_H_ */
