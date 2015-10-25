/*
 * ScriptParser.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <StreamLexer.h>
#include <ScriptParser.h>

namespace nextar {

/***************************************************
 * ScriptParser
 */
ScriptParser::ScriptParser() {
}

ScriptParser::~ScriptParser() {
}

bool ScriptParser::ParseScript(ScriptListener* listener,
		const String& scriptName, InputStreamPtr& input) {

	if (!listener)
		return false;
	const void* readOnlyBuffer = 0;
	size_t readSize = input->AcquireBuffer(readOnlyBuffer);

	StreamLexer lexer(static_cast<const char*>(readOnlyBuffer), readSize);
	ScriptContext context(scriptName, lexer);
	listener->EnterScript(context);
	input->ReleaseBuffer(readOnlyBuffer);

	if (context.IsErrorBitSet()) {
		Error("Errors in script: " + scriptName + ": " + context.GetErrors());
		return false;
	}
	
	return true;
}
/***************************************************
 * ScriptParser::ScriptContext
 */
ScriptParser::ScriptContext::ScriptContext(const String& _scriptName,
		StreamLexer& _lexer) :
		scriptName(_scriptName), lexer(_lexer), blockLevel(0) {
}

void ScriptParser::ScriptContext::ParseRegions(RegionListener* listener) {
	RegionContext context(*this);

	/* read individual regions */
	while (!lexer.IsEndOfStream() && !IsErrorBitSet()) {
		lexer.SkipWhite();
		switch (lexer.Current()) {
		case '@':
			lexer.Forward();
			context.name = lexer.ReadWord(" {\t\n\r");
			if (listener)
				listener->EnterRegion(context);
			break;
		default:
			context.name = "global";
			if (listener)
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
ScriptParser::ContextBase::ContextBase(ScriptContext& scriptCtx) :
		scriptContext(scriptCtx) {
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
	while (!scriptContext.lexer.IsEndOfStream()) {
		switch (scriptContext.lexer.Forward()) {
		case '@':
			scriptContext.lexer.Backward();
			break;
		case '\\':
			if (scriptContext.lexer.Current() == '@') {
				// skip the last char, so split up
				// todo check for buffer overruns inside AsText with signed/unsigned stuff
				size_t s = scriptContext.lexer.Tell();
				scriptContext.lexer.Forward();
				str += scriptContext.lexer.AsText(c, s - c - 1);
				c = s;
			}
		default:
			continue;
		}
		break;
	}
	// todo check for buffer overruns inside AsText with signed/unsigned stuff
	str += scriptContext.lexer.AsText(c, scriptContext.lexer.Tell() - c);
}

void ScriptParser::RegionContext::ParseStatements(StatementListener* listener) {
	BlockContext context(scriptContext);
	context.ParseStatements(listener);
}

void ScriptParser::RegionContext::Discard() {
	while (!scriptContext.lexer.IsEndOfStream()) {
		switch (scriptContext.lexer.Forward()) {
		case '@':
			scriptContext.lexer.Backward();
			break;
		case '\\':
			if (scriptContext.lexer.Current() == '@') {
				scriptContext.lexer.Forward();
			}
			/* no break */
		default:
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

	while (!scriptContext.lexer.IsEndOfStream() && !IsErrorBitSet()) {
		scriptContext.lexer.SkipWhite();
		switch (scriptContext.lexer.Current()) {
		case ';':
			// empty statement
			scriptContext.lexer.Forward();
			continue;
		case '@':
			break;
		case '}':
		case 0:
			scriptContext.DecLevel();
			return;
		case '/': // comment
			scriptContext.lexer.Forward();
			if (scriptContext.lexer.Current() == '/') {
				scriptContext.lexer.SkipLine();
				continue;
			} else
				scriptContext.lexer.Backward();
				/* no break */
		default:
			statement._Clear();
			statement.command = scriptContext.lexer.ReadWord(" {\t\n\r");
			if (statement.command.length() <= 0) {
				scriptContext.Error("expected command name.");
				break;
			}
			bool terminated = false;
			bool wellFormed = false;
			while (!scriptContext.lexer.IsEndOfStream()) {
				// ParameterContext
				scriptContext.lexer.SkipWhite();
				switch (scriptContext.lexer.Current()) {
				case '"': {
					scriptContext.lexer.Forward();
					const String& word = scriptContext.lexer.ReadWord("\"");
					scriptContext.lexer.Forward();
					if (word.length() > 0)
						StringUtils::PushBackWord(statement.paramContext,
						word);
					else
						break;
				}
				continue;
				case ';':
					terminated = true;
					scriptContext.lexer.Forward();
					/* no break */
				case '{':
					wellFormed = true;
					break;
				default: {
					const String& word = scriptContext.lexer.ReadWord();
					if (word.length() > 0)
						StringUtils::PushBackWord(statement.paramContext,
							word);
					else
						break;
				}
				continue;
				}
				break;
			}

			uint32 savedLevel = scriptContext.GetLevel();
			if (!wellFormed) {
				scriptContext.Error(
						"malformed statement, expected ';' or '{'.");
			} else if (!terminated)
				scriptContext.IncLevel();


			if (listener)
				listener->EnterStatement(statement);

			uint32 curLevel = scriptContext.GetLevel();
			while (savedLevel != curLevel &&
				  !scriptContext.lexer.IsEndOfStream() && !IsErrorBitSet()) {
				// the listener did not respond well to the statement, we should skip
				// the parts which did not work and continue parsing
				scriptContext.lexer.Forward();
				if (scriptContext.lexer.Current() == '}')
					--curLevel;
				else if (scriptContext.lexer.Current() == '{')
					++curLevel;
			}
			continue;
		}
		break;
	}
}

/***************************************************
 * ScriptParser::StatementContext
 */
ScriptParser::StatementContext::StatementContext(BlockContext& block,
		ScriptContext& context) :
		ContextBase(context), parentBlockContext(block) {

}

const String& ScriptParser::StatementContext::GetCommand() const {
	return command;
}

const StringUtils::WordList& ScriptParser::StatementContext::GetParamList() const {
	return this->paramContext;
}

void ScriptParser::StatementContext::ParseBlock(BlockListener* listener) {
	BlockContext blockContext(scriptContext);
	int openCount = 0;
	while (!scriptContext.lexer.IsEndOfStream() && !IsErrorBitSet()) {
		scriptContext.lexer.SkipWhite();
		switch (scriptContext.lexer.Forward()) {
		case '{':
			openCount++;
			if (listener)
				listener->EnterBlock(blockContext);
			break;
		case '}':
			if (!(--openCount))
				return;
			if (openCount < 0)
				scriptContext.Error(
				"unexpected '}'");
			break;
		}
	}
}

String ScriptParser::StatementContext::GetTaggedParamVal(const String& tag, 
	StringUtils::TokenIterator it) {
	String value;
	while (it.HasNext(value)) {
		if (value.length()) {
			StringPair tagVal = StringUtils::Split(value, ':');
			if (tagVal.first == tag) {
				return tagVal.second;
			}
		}
	}
	return StringUtils::Null;
}

void ScriptParser::StatementContext::_Clear() {
	command.clear();
	paramContext.clear();
}

} /* namespace nextar */
