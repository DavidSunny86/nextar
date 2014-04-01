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

	struct ProgramCmd :
		public CommandDelegate {
		static ProgramCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);

	};

	struct ProgramListener :
			public ScriptParser::BlockListener,
			public ScriptParser::StatementListener {
		static CommandNamePair commands[];
		static const size_t commandCount;
		ShaderAsset::StreamRequest* shader;
		inline ProgramListener(ShaderAsset::StreamRequest* s) : shader(s) {}
		virtual void EnterBlock(ScriptParser::BlockContext& block);
		virtual void EnterStatement(ScriptParser::StatementContext& statement);
	};

	struct ParamCmd :
			public CommandDelegate {
		static ParamCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

	struct ShaderCmd :
			public CommandDelegate {
		static ShaderCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

} /* namespace ShaderCompiler */
#endif /* PROGRAMCMD_H_ */
