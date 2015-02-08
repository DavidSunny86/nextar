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
class BlendStateListener: public ScriptParser::BlockListener,
		public ScriptParser::StatementListener {
public:
	static CommandNamePair commands[];
	static const size_t commandCount;
	BlendState state;
	
	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);

	static void BlendCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void BlendTargetCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);

protected:
	friend class PassListener;
	~BlendStateListener() {
	}
};

}

#endif /* BLENDCMD_H_ */
