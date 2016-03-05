/*
 * LanguageTranslator.h
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERTRANSLATOR_H_
#define PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERTRANSLATOR_H_

#include <ShaderScript.h>

namespace ShaderScript {

class ShaderScriptContext;

class LanguageTranslatorIntf {
public:

	/** Macro operator */
	virtual void AddPredefs(ShaderScriptContext* script, Pass::ProgramStage stage) = 0;
	virtual void AddMacro(ShaderScriptContext* script, const String& name) = 0;
	/** Constant buffer operator */
	virtual void BeginBuffer(ShaderScriptContext* script, const String& name) = 0;
	virtual void AddParam(ShaderScriptContext* script, ParamDataType dataType, const String& name, uint32 arrayCount) = 0;
	virtual void EndBuffer(ShaderScriptContext* script) = 0;

	virtual ~LanguageTranslatorIntf() {}
};

class LanguageTranslator : public LanguageTranslatorIntf {
public:

	NEX_SINGLE_INSTANCE(LanguageTranslator);

	enum {
		GLSL_TRANSLATOR = 0,
		HLSL_TRANSLATOR,
		TRANSLATOR_COUNT,
	};

	LanguageTranslator();
	virtual ~LanguageTranslator();

	class CmdConstBuffer :public BlockCommandHandler {
	public:
		NEX_SINGLE_INSTANCE(CmdConstBuffer);
		virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
		virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;
	};

	class CmdVar : public CommandHandler {
		NEX_LOG_HELPER(CmdVar);
	protected:
		NEX_SINGLE_INSTANCE(CmdVar);
		virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	};

	virtual void AddMacro(ShaderScriptContext* script, const String& name);
	virtual void AddPredefs(ShaderScriptContext* script, Pass::ProgramStage stage);

	virtual void BeginBuffer(ShaderScriptContext* script, const String& name);
	virtual void AddParam(ShaderScriptContext* script, ParamDataType dataType, const String& name, uint32 arrayCount);
	virtual void EndBuffer(ShaderScriptContext* script);

	void TranslateConstantBuffer(ShaderScriptContext* script, const String& name, nextar::InputStreamPtr);
	void TranslateMacro(ShaderScriptContext* script, const String& name);

protected:
	typedef array<LanguageTranslatorIntf*, TRANSLATOR_COUNT>::type ImplList;

	ImplList translators;
};

} /* namespace ShaderCompiler */

#endif /* PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERTRANSLATOR_H_ */
