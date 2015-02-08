/*
 * MaterialListener.h
 *
 *  Created on: 26-Jul-2014
 *      Author: obhi
 */

#ifndef MATERIALLISTENER_H_
#define MATERIALLISTENER_H_

namespace MaterialCompiler {

class MaterialScript;

class Commands {

protected:

};

/*
 *
 */
class MaterialListener:
		public ScriptParser::StatementListener,
		public ScriptParser::BlockListener {
	MaterialScript* script;
public:
	static CommandNamePair commands[];


	static void ShaderCmd_Execute(void* parentParam,
		ScriptParser::StatementContext& statement);
	static void OptionsCmd_Execute(void* parentParam,
		ScriptParser::StatementContext& statement);
	static void ParamValueCmd_Execute(void* parentParam,
		ScriptParser::StatementContext& statement);
	static void LayerCmd_Execute(void* parentParam,
		ScriptParser::StatementContext& statement);

	static const size_t commandCount;

	MaterialListener(MaterialScript* _script) : script(_script) {
	}

	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);

protected:
	friend class MaterialScript;
	~MaterialListener() {}
};

} /* namespace MaterialCompiler */

#endif /* MATERIALLISTENER_H_ */
