/*
 * ConstBufferCmd.cpp
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#include <ConstBufferCmd.h>

namespace ShaderCompiler {

ConstBufferCmd::ConstBufferCmd() {

}

ConstBufferCmd::~ConstBufferCmd() {

}

void ConstBufferCmd::Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& ctx) {

	ShaderScript* script = static_cast<ShaderScript*>(parentParam);
	String name;
	StringUtils::NextWord(ctx.GetParamList(), name);

	InputStreamPtr file = script->FetchConstBuffer(name);

	ConstBufferTranslator translator;
	translator.Translate(script, name, file);
}

} /* namespace ShaderCompiler */

