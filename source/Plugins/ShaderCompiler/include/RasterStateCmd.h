/*
 * RasterCmd.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef RASTERCMD_H_
#define RASTERCMD_H_

#include "CommonTypes.h"

namespace ShaderCompiler {
	/** BlendState {} */
	struct RasterStateCmd :
			public CommandDelegate {
		static RasterStateCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

	struct RasterStateListener :
			public ScriptParser::BlockListener,
			public ScriptParser::StatementListener {
		static CommandNamePair commands[];
		static const size_t commandCount;
		RasterState state;

		virtual void EnterBlock(ScriptParser::BlockContext& block);
		virtual void EnterStatement(ScriptParser::StatementContext& statement);
	};

	struct RasterCmd :
			public CommandDelegate {
		static RasterCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

	struct ScissorCmd :
			public CommandDelegate {
		static ScissorCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

	struct AntiAliasingCmd :
			public CommandDelegate {
		static AntiAliasingCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

	struct DepthClipCmd :
			public CommandDelegate {
		static DepthClipCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

	struct DepthBiasCmd :
			public CommandDelegate {
		static DepthBiasCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};
}



#endif /* RASTERCMD_H_ */
