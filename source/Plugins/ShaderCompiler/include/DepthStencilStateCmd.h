/*
 * DepthStencil.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef DEPTHSTENCIL_H_
#define DEPTHSTENCIL_H_

#include <CommonTypes.h>

namespace ShaderCompiler {

	/** DepthStencil {} */
	struct DepthStencilStateCmd :
			public CommandDelegate {
		static DepthStencilStateCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);

	};

	struct DepthStencilStateListener :
			public ScriptParser::BlockListener,
			public ScriptParser::StatementListener {
		static CommandNamePair commands[];
		static const size_t commandCount;
		DepthStencilState state;

		virtual void EnterBlock(ScriptParser::BlockContext& block);
		virtual void EnterStatement(ScriptParser::StatementContext& statement);
	};

	struct DepthStateCmd :
			public CommandDelegate {
		static DepthStateCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

	struct StencilTestCmd :
			public CommandDelegate {
		static StencilTestCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

	struct StencilOpCmd :
			public CommandDelegate {
		static StencilOpCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};

} /* namespace nextar */
#endif /* DEPTHSTENCIL_H_ */
