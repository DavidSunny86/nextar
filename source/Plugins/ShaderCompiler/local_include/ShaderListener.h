/*
 * ShaderListener.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef SHADERLISTENER_H_
#define SHADERLISTENER_H_

#include <CommonTypes.h>

namespace ShaderCompiler {

class ShaderScript;
/** Shader <name> {}*/
class ShaderListener: public ScriptParser::StatementListener,
		public ScriptParser::BlockListener {
public:
	static CommandNamePair commands[];
	static const size_t commandCount;

	String name;
	ShaderScript* shaderScript;

	ShaderListener(ShaderScript*, const String& name);

	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);
protected:
	friend class ShaderScript;
	~ShaderListener() {
	}
};
}

#endif /* SHADERLISTENER_H_ */
