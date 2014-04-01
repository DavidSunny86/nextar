/*
 * BlendCmd.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef BLENDCMD_H_
#define BLENDCMD_H_

#include <CommonTypes.h>
#include <BlendStateCmd.h>

namespace ShaderCompiler {
	/** BlendState {} */
	struct BlendStateCmd :
			public CommandDelegate {
		static BlendStateCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);

	};

	struct BlendStateListener :
			public ScriptParser::BlockListener,
			public ScriptParser::StatementListener {
		static CommandNamePair commands[];
		static const size_t commandCount;
		BlendState state;

		virtual void EnterBlock(ScriptParser::BlockContext& block);
		virtual void EnterStatement(ScriptParser::StatementContext& statement);
	};

	struct BlendCmd :
			public CommandDelegate {
		static BlendCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

	struct BlendTargetCmd :
			public CommandDelegate {
		static BlendTargetCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

}


#endif /* BLENDCMD_H_ */
