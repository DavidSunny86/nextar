/*
 * ConstBufferGLSL.h
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERHLSL_H_
#define PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERHLSL_H_

#include <LanguageTranslator.h>

namespace ShaderScript {

class TranslatorHLSL :
		public AllocGeneral,
		public LanguageTranslatorIntf {
public:
	NEX_SINGLE_INSTANCE(TranslatorHLSL);
	TranslatorHLSL();
	virtual ~TranslatorHLSL();

	virtual void BeginBuffer(ShaderScriptContext* script, const String& name);
	virtual void AddParam(ShaderScriptContext* script, ParamDataType dataType, const String& name, uint32 arrayCount);
	virtual void EndBuffer(ShaderScriptContext* _script);

	virtual void AddMacro(ShaderScriptContext* script, const String& name);
	virtual void AddPredefs(ShaderScriptContext* script, Pass::ProgramStage stage);

protected:
};

} /* namespace ShaderCompiler */

#endif /* PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERHLSL_H_ */
