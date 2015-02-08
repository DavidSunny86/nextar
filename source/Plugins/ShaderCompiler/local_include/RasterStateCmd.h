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
/** RasterState {} */
class RasterStateListener: public ScriptParser::BlockListener,
		public ScriptParser::StatementListener {
public:
	static CommandNamePair commands[];
	static const size_t commandCount;
	RasterState state;

	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);

	static void RasterCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void ScissorCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void AntiAliasingCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void DepthClipCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void DepthBiasCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
protected:
	friend class PassListener;
	~RasterStateListener() {
	}
};


}

#endif /* RASTERCMD_H_ */
