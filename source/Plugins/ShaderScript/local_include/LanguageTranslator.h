/*
 * LanguageTranslator.h
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_LanguageTranslator_H_
#define PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_LanguageTranslator_H_

#include <ShaderScript.h>

namespace ShaderScript {

class ShaderScriptContext;

class LanguageTranslatorIntf {
public:
		
	class Context : public CommandContext {
	public:
		ShaderScriptContext* _script;
	};

	/** Macro operator */
	virtual void AddPredefs(ShaderScriptContext* script, Pass::ProgramStage stage) = 0;
	virtual void AddMacro(ShaderScriptContext* script, const String& name) = 0;
	/** Constant buffer operator */
	virtual void BeginBuffer(ShaderScriptContext* script, const String& name, const String& modName) = 0;
	virtual void AddParam(ShaderScriptContext* script, ParamDataType dataType, const String& name, uint32 arrayCount) = 0;
	virtual void EndBuffer(ShaderScriptContext* script) = 0;

	virtual ~LanguageTranslatorIntf() {}
};

class LanguageTranslator : public LanguageTranslatorIntf {
	NEX_LOG_HELPER(LanguageTranslator);
public:


	NEX_SINGLE_INSTANCE(LanguageTranslator);

public:
	virtual void AddMacro(ShaderScriptContext* script, const String& name);
	virtual void AddPredefs(ShaderScriptContext* script, Pass::ProgramStage stage);
	void TranslateConstantBuffer(ShaderScriptContext* script, const String& name, nextar::InputStreamPtr);
	void TranslateMacro(ShaderScriptContext* script, const String& name);

	LanguageTranslator();
	virtual ~LanguageTranslator();

	static bool ConstBuffer_BeginExecute(ShaderScriptContext* script, const ASTCommand* command);
	static void ConstBuffer_EndExecute(ShaderScriptContext* script, const ASTCommand* command);
	static bool Declare_BeginExecute(ShaderScriptContext* script, const ASTCommand* command);

	class CmdConstBuffer :public BlockCommandHandler {
	public:
		NEX_SINGLE_INSTANCE(CmdConstBuffer);
		virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
		virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;
	};

	class CmdDeclare : public CommandHandler {
	public:
		NEX_SINGLE_INSTANCE(CmdDeclare);
		virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	};

protected:
	virtual void BeginBuffer(ShaderScriptContext* script, const String& name, const String& modName);
	virtual void AddParam(ShaderScriptContext* script, ParamDataType dataType, const String& name, uint32 arrayCount);
	virtual void EndBuffer(ShaderScriptContext* script);


protected:
	typedef array<LanguageTranslatorIntf*, TRANSLATOR_COUNT>::type ImplList;

	ImplList translators;
};

} /* namespace ShaderCompiler */

#endif /* PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_LanguageTranslator_H_ */
