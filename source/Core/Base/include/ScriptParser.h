/*
 * ScriptParser.h
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#ifndef SCRIPTPARSER_H_
#define SCRIPTPARSER_H_

#include "NexBase.h"
#include "StringUtils.h"

namespace nextar {

	class StreamLexer;
	class _NexBaseExport ScriptParser {
		NEX_LOG_HELPER(ScriptParser);
	public:

		class ScriptContext;
		class BlockListener;
		class BlockContext;
		class StatementListener;
		class StatementContext;
		class RegionContext;

		struct StatementListener {
			virtual void EnterStatement(ScriptParser::StatementContext& statement) {}
		};

		struct RegionListener {
			virtual void EnterRegion(ScriptParser::RegionContext& regionCtx) {}
		};

		struct BlockListener {
			virtual void EnterBlock(ScriptParser::BlockContext& block) {}
		};

		struct ScriptListener {
			virtual void EnterScript(ScriptParser::ScriptContext& block) {}
		};

		class ContextBase {
		public:
			ContextBase(ScriptContext& scriptContext);

			void Error(const String& error);
			bool IsErrorBitSet() const;
		protected:
			ScriptContext& scriptContext;
		};

		class RegionContext: public ContextBase {
			friend class ScriptContext;
		public:
			RegionContext(ScriptContext& context);

			/* Returns the complete text under this region */
			void ParseText(String&);
			void ParseStatements(StatementListener*);
			void Discard();

		protected:
			String name;
		};

		class StatementContext : public ContextBase {
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

		class BlockContext : public ContextBase {
			friend class ScriptContext;
			friend class StatementContext;
		public:
			BlockContext(ScriptContext& scriptContex);

			void ParseStatements(StatementListener*);
		protected:

		};

		class ScriptContext {
			friend class BlockContext;
			friend class StatementContext;
			friend class RegionContext;
		public:
			ScriptContext(const String& scriptName, StreamLexer& lexer);

			void ParseRegions(RegionListener*);
			void Error(const String& error);
			bool IsErrorBitSet() const;
			const String& GetErrors() const;
		protected:
			StreamLexer& lexer;
			const String& scriptName;
			String errorLog;
		};

		ScriptParser();
		virtual ~ScriptParser();

		void ParseScript(ScriptListener* listener, const String& scriptName, InputStreamPtr& input);
	};

} /* namespace nextar */
#endif /* SCRIPTPARSER_H_ */
