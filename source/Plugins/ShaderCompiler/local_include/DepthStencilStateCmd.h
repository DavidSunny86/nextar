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
class DepthStencilStateCmd: public CommandDelegate {
public:
	static DepthStencilStateCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~DepthStencilStateCmd() {
	}

};

class DepthStencilStateListener: public ScriptParser::BlockListener,
		public ScriptParser::StatementListener {
public:
	static CommandNamePair commands[];
	static const size_t commandCount;
	DepthStencilState state;

	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);
protected:
	friend class DepthStencilStateCmd;
	~DepthStencilStateListener() {
	}
};

class DepthStateCmd: public CommandDelegate {
public:
	static DepthStateCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~DepthStateCmd() {
	}
};

class StencilTestCmd: public CommandDelegate {
public:
	static StencilTestCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~StencilTestCmd() {
	}
};

class StencilOpCmd: public CommandDelegate {
public:
	static StencilOpCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~StencilOpCmd() {
	}
};

} /* namespace nextar */
#endif /* DEPTHSTENCIL_H_ */
