/*
 * ShaderCompiler.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <ShaderListener.h>
#include <PassCmd.h>
#include <ShaderScript.h>
#include <ScriptStrings.h>

namespace ShaderCompiler {

FlagsCmd FlagsCmd::command;
/**************************************************************
 * Command Map
 **************************************************************/
CommandNamePair ShaderListener::commands[] = {
		{ _SS(CMD_FLAGS), &FlagsCmd::command },
		{ _SS(CMD_PASS), &PassCmd::command },
};

const size_t ShaderListener::commandCount = sizeof(ShaderListener::commands)
		/ sizeof(ShaderListener::commands[0]);

/**************************************************************
 * ShaderListener
 **************************************************************/
ShaderListener::ShaderListener(ShaderScript* sh, const String& _name) :
		shaderScript(sh), name(_name) {
}

void ShaderListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void ShaderListener::EnterStatement(ScriptParser::StatementContext& ctx) {
	CommandDelegate* cmd = Helper::FindCommand(ShaderListener::commands,
			ShaderListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd->Execute(CommandDelegate::SHADER_BLOCK, shaderScript, ctx);
	// todo else throw error command not supported
}

/**************************************************************
 * FlagsCmd
 **************************************************************/
void FlagsCmd::Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegate::SHADER_BLOCK) {
		const StringUtils::WordList& wl = ctx.GetParamList();
		String value;
		StringUtils::TokenIterator prev = 0;
		uint32 flags = 0;
		while (	(prev = StringUtils::NextWord(wl, value, prev)) != String::npos ) {
			flags |= Helper::GetShaderFlag(value);
		}

		if (flags) {
			ShaderTemplate* shader = static_cast<ShaderTemplate*>(
					static_cast<ShaderScript*>(parentParam)->GetRequest()->GetStreamedObject());
			shader->SetRenderFlags(flags);
		}
	} else {
		ctx.Error("BlendState block needs to be inside Shader declaration.");
	}
}

}
