/*
 * ScriptParser.h
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#ifndef SCRIPTPARSER_H_
#define SCRIPTPARSER_H_

#include <NexBase.h>
#include <StringUtils.h>

namespace nextar {

class StreamLexer;
class _NexBaseAPI ScriptParser {
	NEX_LOG_HELPER(ScriptParser)
	;
public:

	class ScriptContext;
	class BlockListener;
	class BlockContext;
	class StatementListener;
	class StatementContext;
	class RegionContext;

	class _NexBaseAPI StatementListener {
	public:
		virtual void EnterStatement(ScriptParser::StatementContext& statement) {
		}
	protected:
		~StatementListener() {}
	};

	class _NexBaseAPI RegionListener {
	public:
		virtual void EnterRegion(ScriptParser::RegionContext& regionCtx) {
		}
	protected:
		~RegionListener() {}
	};

	class _NexBaseAPI BlockListener {
	public:
		virtual void EnterBlock(ScriptParser::BlockContext& block) {
		}
	protected:
		~BlockListener() {}
	};

	class _NexBaseAPI ScriptListener {
	public:
		virtual void EnterScript(ScriptParser::ScriptContext& block) {
		}
	protected:
		~ScriptListener() {}
	};

	class _NexBaseAPI ContextBase {
	public:
		ContextBase(ScriptContext& scriptContext);

		void Error(const String& error);bool IsErrorBitSet() const;
	protected:
		ScriptContext& scriptContext;
	};

	class _NexBaseAPI RegionContext: public ContextBase {
		friend class ScriptContext;
	public:
		RegionContext(ScriptContext& context);

		inline const String& GetName() const {
			return name;
		}

		/* Returns the complete text under this region */
		void ParseText(String&);
		void ParseStatements(StatementListener*);
		void Discard();

	protected:
		String name;
	};

	class _NexBaseAPI StatementContext: public ContextBase {
		friend class BlockContext;
		friend class ScriptContext;
	public:
		StatementContext(BlockContext& block, ScriptContext& context);
		const String& GetCommand() const;
		const StringUtils::WordList& GetParamList() const;

		void ParseBlock(BlockListener*);
	protected:
		void _Clear();

		BlockContext& parentBlockContext;

		String command;
		StringUtils::WordList paramContext;
	};

	class _NexBaseAPI BlockContext: public ContextBase {
		friend class ScriptContext;
		friend class StatementContext;
	public:
		BlockContext(ScriptContext& scriptContex);

		void ParseStatements(StatementListener*);
	protected:

	};

	class _NexBaseAPI ScriptContext {
		friend class BlockContext;
		friend class StatementContext;
		friend class RegionContext;
	public:
		ScriptContext(const String& scriptName, StreamLexer& lexer);

		void ParseRegions(RegionListener*);
		void Error(const String& error);bool IsErrorBitSet() const;
		const String& GetErrors() const;

	protected:
		StreamLexer& lexer;
		const String& scriptName;
		String errorLog;
	};

	ScriptParser();
	virtual ~ScriptParser();

	bool ParseScript(ScriptListener* listener, const String& scriptName,
			InputStreamPtr& input);
};

} /* namespace nextar */
#endif /* SCRIPTPARSER_H_ */
