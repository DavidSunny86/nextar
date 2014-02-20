/*
 * ScriptParser.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "StreamLexer.h"
#include "ScriptParser.h"

namespace nextar {


	/***************************************************
	 * ScriptParser
	 */
	ScriptParser::ScriptParser() {
	}

	ScriptParser::~ScriptParser() {
	}

	void ScriptParser::ParseScript(ScriptListener* listener,
			const String& scriptName, InputStreamPtr& input) {

		const void* readOnlyBuffer = 0;
		size_t readSize = input->AcquireBuffer(readOnlyBuffer);

		StreamLexer lexer(static_cast<const char*>(readOnlyBuffer), readSize);
		ScriptContext context(scriptName, lexer);
		listener->EnterScript(context);

		if (context.IsErrorBitSet()) {
			Error("Errors in script: " + scriptName + ": " + context.GetErrors());
		}
		input->ReleaseBuffer(readOnlyBuffer);
	}
	/***************************************************
	 * ScriptParser::ScriptContext
	 */
	ScriptParser::ScriptContext::ScriptContext(const String& _scriptName, StreamLexer& _lexer) :
			scriptName(_scriptName), lexer(_lexer) {
	}

	void ScriptParser::ScriptContext::ParseRegions(RegionListener* listener) {
		RegionContext context(*this);

		/* read individual regions */
		while (!lexer.IsEndOfStream()) {
			lexer.SkipWhite();
			switch(lexer.Current()) {
			case '@':
				lexer.Forward();
				context.name = lexer.ReadWord(" {\t\n\r");
				listener->EnterRegion(context);
				break;
			default:
				context.name = "global";
				listener->EnterRegion(context);
				break;
			}
		}
	}

	void ScriptParser::ScriptContext::Error(const String& error) {
		errorLog += error;
		errorLog += "@Line:";
		errorLog += Convert::ToString(lexer.Line());
	}

	bool ScriptParser::ScriptContext::IsErrorBitSet() const {
		return errorLog.length() > 0;
	}

	const String& ScriptParser::ScriptContext::GetErrors() const {
		return errorLog;
	}
	/***************************************************
	 * ScriptParser::ContextBase
	 */
	ScriptParser::ContextBase::ContextBase(ScriptContext& scriptCtx) : scriptContext(scriptCtx) {
	}

	void ScriptParser::ContextBase::Error(const String& err) {
		scriptContext.Error(err);
	}

	bool ScriptParser::ContextBase::IsErrorBitSet() const {
		return scriptContext.IsErrorBitSet();
	}
	/***************************************************
	 * ScriptParser::RegionContext
	 */
	ScriptParser::RegionContext::RegionContext(ScriptContext& _scriptContext) :
				ContextBase(_scriptContext) {
	}

	void ScriptParser::RegionContext::ParseText(String& str) {
		size_t c = scriptContext.lexer.Tell();
		while(!scriptContext.lexer.IsEndOfStream()) {
			switch(scriptContext.lexer.Forward()) {
			case '@':
				break;
			case '\\':
				if (scriptContext.lexer.Current() == '@') {
					// skip the last char, so split up
					// todo check for buffer overruns inside AsText with signed/unsigned stuff
					size_t s = scriptContext.lexer.Tell();
					scriptContext.lexer.Forward();
					str += scriptContext.lexer.AsText(c, s-c-1);
					c = s;
				}
				continue;
			}
			break;
		}
		// todo check for buffer overruns inside AsText with signed/unsigned stuff
		str += scriptContext.lexer.AsText(c, scriptContext.lexer.Tell()-c);
	}

	void ScriptParser::RegionContext::ParseStatements(StatementListener* listener) {
		BlockContext context(scriptContext);
		context.ParseStatements(listener);
	}

	void ScriptParser::RegionContext::Discard() {
		while(!scriptContext.lexer.IsEndOfStream()) {
			switch(scriptContext.lexer.Forward()) {
			case '@':
				break;
			case '\\':
				if (scriptContext.lexer.Current() == '@') {
					scriptContext.lexer.Forward();
				}
				continue;
			}
			break;
		}
	}
	/***************************************************
	 * ScriptParser::BlockContext
	 */
	ScriptParser::BlockContext::BlockContext(ScriptContext& _scriptContext) :
			ContextBase(_scriptContext) {
	}

	void ScriptParser::BlockContext::ParseStatements(StatementListener* listener) {
		StatementContext statement(*this, scriptContext);

		while (!scriptContext.lexer.IsEndOfStream()) {
			scriptContext.lexer.SkipWhite();
			switch(scriptContext.lexer.Current()) {
			case '@':
				break;
			case '}':
			case 0:
				continue;
			case '/': // comment
				scriptContext.lexer.Forward();
				if (scriptContext.lexer.Current() == '/') {
					scriptContext.lexer.SkipLine();
					continue;
				}
				else
					scriptContext.lexer.Backward();
				continue;
			default:
				statement._Clear();
				statement.command = scriptContext.lexer.ReadWord(" {\t\n\r");
				if (statement.command.length() <= 0) {
					scriptContext.Error(
							"expected command name.");
					break;
				}
				bool wellFormed = false;
				while (!scriptContext.lexer.IsEndOfStream()) {
					// ParameterContext
					switch(scriptContext.lexer.Current()) {
					case '{':
					case ';':
						wellFormed = true;
						break;
					default:
						StringUtils::PushBackWord(statement.paramContext,
								scriptContext.lexer.ReadWord());
						continue;
					}
					break;
				}
				if (!wellFormed) {
					scriptContext.Error(
						"malformed statement, expected ';' or '{'.");
				}
				listener->EnterStatement(statement);

				continue;
			}
			break;
		}
	}

	/***************************************************
	 * ScriptParser::StatementContext
	 */
	ScriptParser::StatementContext::StatementContext(BlockContext& block,
			ScriptContext& context) : ContextBase(context), parentBlockContext(block) {

	}

	const String& ScriptParser::StatementContext::GetCommand() const {
		return command;
	}

	const StringUtils::WordList& ScriptParser::StatementContext::GetParamList() const {
		return this->paramContext;
	}

	void ScriptParser::StatementContext::ParseBlock(BlockListener* listener) {
		BlockContext blockContext(scriptContext);
		while (!scriptContext.lexer.IsEndOfStream()) {
			scriptContext.lexer.SkipWhite();
			switch(scriptContext.lexer.Current()) {
			case '{':
				listener->EnterBlock(blockContext);
				break;
			case '}':
				return;
			default:
				scriptContext.Error("expected '{' or '}', got: " + String(scriptContext.lexer.Current()));
				break;
			}
		}
	}

	void ScriptParser::StatementContext::_Clear() {
		command.clear();
		paramContext.clear();
	}
} /* namespace nextar */
