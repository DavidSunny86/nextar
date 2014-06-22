/*
 * RasterCmd.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef RASTERCMD_H_
#define RASTERCMD_H_

#include <CommonTypes.h>

namespace ShaderCompiler {
/** BlendState {} */
class RasterStateCmd: public CommandDelegate {
public:
	static RasterStateCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~RasterStateCmd() {
	}
};

class RasterStateListener: public ScriptParser::BlockListener,
		public ScriptParser::StatementListener {
public:
	static CommandNamePair commands[];
	static const size_t commandCount;
	RasterState state;

	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);
protected:
	~RasterStateListener() {
	}
};

class RasterCmd: public CommandDelegate {
public:
	static RasterCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~RasterCmd() {
	}
};

class ScissorCmd: public CommandDelegate {
public:
	static ScissorCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~ScissorCmd() {
	}
};

class AntiAliasingCmd: public CommandDelegate {
public:
	static AntiAliasingCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~AntiAliasingCmd() {
	}
};

class DepthClipCmd: public CommandDelegate {
public:
	static DepthClipCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~DepthClipCmd() {
	}
};

class DepthBiasCmd: public CommandDelegate {
public:
	static DepthBiasCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
protected:
	~DepthBiasCmd() {
	}
};
}

#endif /* RASTERCMD_H_ */
