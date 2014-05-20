/*
 * ProgramCmd.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef PROGRAMCMD_H_
#define PROGRAMCMD_H_

#include <CommonTypes.h>

namespace ShaderCompiler {

	class ProgramCmd :
		public CommandDelegate {
	public:
		static ProgramCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	protected:
		~ProgramCmd() {}
	};

	class ProgramListener :
			public ScriptParser::BlockListener,
			public ScriptParser::StatementListener {
	public:
		static CommandNamePair commands[];
		static const size_t commandCount;
		ShaderScript* shaderScript;
		inline ProgramListener(ShaderScript* s) : shaderScript(s) {}
		virtual void EnterBlock(ScriptParser::BlockContext& block);
		virtual void EnterStatement(ScriptParser::StatementContext& statement);
	protected:
		~ProgramListener() {}
	};

	class ParamCmd :
			public CommandDelegate {
	public:
		static ParamCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	protected:
		~ParamCmd() {}
	};

	class ShaderCmd :
			public CommandDelegate {
	public:
		static ShaderCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	protected:
		~ShaderCmd() {}
	};

} /* namespace ShaderCompiler */
#endif /* PROGRAMCMD_H_ */
