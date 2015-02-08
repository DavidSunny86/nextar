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
class DepthStencilStateListener: public ScriptParser::BlockListener,
		public ScriptParser::StatementListener {
public:
	static CommandNamePair commands[];
	static const size_t commandCount;
	DepthStencilState state;

	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);

	static void DepthStateCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void StencilTestCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void StencilOpCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);

protected:
	friend class PassListener;
	~DepthStencilStateListener() {
	}
};

} /* namespace nextar */
#endif /* DEPTHSTENCIL_H_ */
