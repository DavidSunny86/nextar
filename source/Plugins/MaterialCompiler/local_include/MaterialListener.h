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

class ShaderCmd: public CommandDelegate {
public:

	static ShaderCmd command;
	virtual void Execute(void* parentParam,
				ScriptParser::StatementContext& statement);
protected:

};

class OptionsCmd: public CommandDelegate {
public:

	static OptionsCmd command;
	virtual void Execute(void* parentParam,
				ScriptParser::StatementContext& statement);
protected:

};

class ParamValueCmd: public CommandDelegate {
public:

	static ParamValueCmd command;
	virtual void Execute(void* parentParam,
				ScriptParser::StatementContext& statement);
protected:

};

class LayerCmd: public CommandDelegate {
public:

	static LayerCmd command;
	virtual void Execute(void* parentParam,
				ScriptParser::StatementContext& statement);
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
