/*
 * PassCmd.h
 *
 *  Created on: 24-Jul-2014
 *      Author: obhi
 */

#ifndef PASSCMD_H_
#define PASSCMD_H_

#include <CommonTypes.h>

namespace ShaderCompiler {

/*
 *
 */
class PassCmd: public CommandDelegate {
public:

	static PassCmd command;
	virtual void Execute(int parentType, void* parentParam,
				ScriptParser::StatementContext& statement);

protected:
	~PassCmd() {
	}
};

class PassListener: public ScriptParser::StatementListener,
public ScriptParser::BlockListener {
public:
	static CommandNamePair commands[];
	static const size_t commandCount;

	String name;
	ShaderScript* shaderScript;

	PassListener(ShaderScript*, const String& name);

	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);

protected:
	friend class ShaderListener;
	friend class PassCmd;
	~PassListener() {
	}
};

} /* namespace ShaderCompiler */

#endif /* PASSCMD_H_ */
