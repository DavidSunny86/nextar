/*
 * ConstBufferCmd.h
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERCMD_H_
#define PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERCMD_H_

#include <CommonTypes.h>

namespace ShaderCompiler {

class ConstBufferTranslator;
class ConstBufferCmd: public CommandDelegate {
public:
	static ConstBufferCmd command;
	ConstBufferCmd();
	virtual ~ConstBufferCmd();

	virtual void Execute(int parentType, void* parentParam,
				ScriptParser::StatementContext& statement);

protected:

};

} /* namespace ShaderCompiler */

#endif /* PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERCMD_H_ */
