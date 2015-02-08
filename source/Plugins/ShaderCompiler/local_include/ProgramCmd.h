/*
 * ProgramCmd.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef PROGRAMCMD_H_
#define PROGRAMCMD_H_

#include <CommonTypes.h>

namespace ShaderCompiler {
class ShaderScript;

class ProgramListener: public ScriptParser::BlockListener,
		public ScriptParser::StatementListener {
public:
	static CommandNamePair commands[];
	static const size_t commandCount;
	ShaderScript* shaderScript;
	inline ProgramListener(ShaderScript* s) :
			shaderScript(s) {
	}
	~ProgramListener() {
	}
	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);

	static void ParamCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);

	static void DomainCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void GeometryCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void HullCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void FragmentCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void VertexCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);

	static void ShaderCmd_Execute(ShaderScript* script, ScriptParser::StatementContext& statement, Pass::ProgramStage stage);
protected:
	friend class ProgramCmd;

};

} /* namespace ShaderCompiler */
#endif /* PROGRAMCMD_H_ */
