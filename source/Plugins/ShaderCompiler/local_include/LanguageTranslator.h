/*
 * LanguageTranslator.h
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERTRANSLATOR_H_
#define PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERTRANSLATOR_H_

#include <CommonTypes.h>

namespace ShaderCompiler {

class ShaderScript;

class LanguageTranslatorIntf {
public:

	/** Macro operator */
	virtual void AddPredefs(Pass::ProgramStage stage, ShaderScript* script) = 0;
	virtual void AddMacro(ShaderScript* script, const String& name) = 0;
	/** Constant buffer operator */
	virtual void BeginBuffer(const String& name) = 0;
	virtual void AddParam(ParamDataType dataType, const String& name, uint32 arrayCount) = 0;
	virtual void EndBuffer(ShaderScript* script) = 0;

	virtual ~LanguageTranslatorIntf() {}
};

class LanguageTranslator : public LanguageTranslatorIntf {
public:

	enum {
		GLSL_TRANSLATOR = 0,
		HLSL_TRANSLATOR,
		TRANSLATOR_COUNT,
	};

	LanguageTranslator();
	virtual ~LanguageTranslator();

	class ConstBufferListener :public ScriptParser::RegionListener,
		public ScriptParser::ScriptListener,
		public ScriptParser::StatementListener {
	public:

		ConstBufferListener(LanguageTranslator* _translator,
				ShaderScript* _script) : script(_script), translator(_translator) {}

		virtual void EnterRegion(ScriptParser::RegionContext& regionCtx);
		virtual void EnterScript(ScriptParser::ScriptContext& block);
		virtual void EnterStatement(ScriptParser::StatementContext& ctx);

	protected:
		LanguageTranslator* translator;
		ShaderScript* script;
	};

	class ConstBufferCommandListener : public ScriptParser::StatementListener,
	public ScriptParser::BlockListener {
	public:
		ConstBufferCommandListener(ShaderScript* _script,
				LanguageTranslator* _translator, bool _cbIsAutoParam) : script(_script),
				translator(_translator), cbIsAutoParam(_cbIsAutoParam) {
		}

		virtual void EnterBlock(ScriptParser::BlockContext& block);
		virtual void EnterStatement(ScriptParser::StatementContext& statement);

	protected:

		bool cbIsAutoParam;
		LanguageTranslator* translator;
		ShaderScript* script;
	};

	virtual void AddMacro(ShaderScript* script, const String& name);
	virtual void AddPredefs(Pass::ProgramStage stage, ShaderScript* script);

	virtual void BeginBuffer(const String& name);
	virtual void AddParam(ParamDataType dataType, const String& name, uint32 arrayCount);
	virtual void EndBuffer(ShaderScript* script);


	void TranslateConstantBuffer(ShaderScript* script, const String& name, nextar::InputStreamPtr);
	void TranslateMacro(ShaderScript* script, const String& name);

protected:
	typedef array<LanguageTranslatorIntf*, TRANSLATOR_COUNT>::type ImplList;

	ImplList translators;
};

} /* namespace ShaderCompiler */

#endif /* PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERTRANSLATOR_H_ */
