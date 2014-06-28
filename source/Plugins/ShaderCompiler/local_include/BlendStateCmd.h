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
class BlendStateCmd: public CommandDelegate {
public:
	static BlendStateCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~BlendStateCmd() {
	}
};

class BlendStateListener: public ScriptParser::BlockListener,
		public ScriptParser::StatementListener {
public:
	static CommandNamePair commands[];
	static const size_t commandCount;
	BlendState state;

	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);
protected:
	friend class BlendStateCmd;
	~BlendStateListener() {
	}
};

class BlendCmd: public CommandDelegate {
public:
	static BlendCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~BlendCmd() {
	}
};

struct BlendTargetCmd: public CommandDelegate {
public:
	static BlendTargetCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~BlendTargetCmd() {
	}
};

}

#endif /* BLENDCMD_H_ */
