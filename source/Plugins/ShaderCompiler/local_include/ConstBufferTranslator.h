/*
 * ConstBufferTranslator.h
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERTRANSLATOR_H_
#define PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERTRANSLATOR_H_

#include <CommonTypes.h>

namespace ShaderCompiler {

class ConstBufferTranslatorImpl {
public:
	virtual void BeginBuffer(const String& name) = 0;
	virtual void AddParam(ParamDataType dataType, const String& name, uint32 arrayCount) = 0;
	virtual void EndBuffer(ShaderScript* script) = 0;
protected:
	~ConstBufferTranslatorImpl() {}
};

class ConstBufferTranslator : public ConstBufferTranslatorImpl {
public:

	enum {
		GLSL_TRANSLATOR = 0,
		HLSL_TRANSLATOR,
		TRANSLATOR_COUNT,
	};

	ConstBufferTranslator();
	virtual ~ConstBufferTranslator();

	class ConstBufferListener :public ScriptParser::RegionListener,
		public ScriptParser::ScriptListener,
		public ScriptParser::StatementListener {
	public:

		ConstBufferListener(ConstBufferTranslator* _translator,
				ShaderScript* _script) : script(_script), translator(_translator) {}

		virtual void EnterRegion(ScriptParser::RegionContext& regionCtx);
		virtual void EnterScript(ScriptParser::ScriptContext& block);
		virtual void EnterStatement(ScriptParser::StatementContext& ctx);

	protected:
		ConstBufferTranslator* translator;
		ShaderScript* script;
	};

	class ConstBufferCommandListener : public ScriptParser::StatementListener,
	public ScriptParser::BlockListener {
	public:
		ConstBufferCommandListener(ShaderScript* _script,
				ConstBufferTranslator* _translator) : script(_script),
				translator(_translator) {
		}

		virtual void EnterBlock(ScriptParser::BlockContext& block);
		virtual void EnterStatement(ScriptParser::StatementContext& statement);

	protected:

		ConstBufferTranslator* translator;
		ShaderScript* script;
	};


	virtual void BeginBuffer(const String& name);
	virtual void AddParam(ParamDataType dataType, const String& name, uint32 arrayCount);
	virtual void EndBuffer(ShaderScript* script);

	void Translate(ShaderScript* script, const String& name, nextar::InputStreamPtr);

protected:
	typedef array<ConstBufferTranslatorImpl*, TRANSLATOR_COUNT>::type ImplList;

	ImplList translators;
};

} /* namespace ShaderCompiler */

#endif /* PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERTRANSLATOR_H_ */
