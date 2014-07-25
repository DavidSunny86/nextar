/*
 * PassCmd.cpp
 *
 *  Created on: 24-Jul-2014
 *      Author: obhi
 */

#include <BaseHeaders.h>
#include <ShaderListener.h>
#include <DepthStencilStateCmd.h>
#include <BlendStateCmd.h>
#include <RasterStateCmd.h>
#include <TextureUnitStateCmd.h>
#include <ShaderScript.h>
#include <ScriptStrings.h>
#include <ProgramCmd.h>
#include <PassCmd.h>

namespace ShaderCompiler {

PassCmd PassCmd::command;

/**************************************************************
 * Command Map
 **************************************************************/
CommandNamePair PassListener::commands[] = {
		{ _SS(CMD_BLEND_STATE), &BlendStateCmd::command },
		{ _SS(CMD_DEPTH_STENCIL_STATE), &DepthStencilStateCmd::command },
		{ _SS(CMD_PROGRAM), &ProgramCmd::command },
		{ _SS(CMD_RASTER_STATE), &RasterStateCmd::command },
};

const size_t PassListener::commandCount = sizeof(PassListener::commands)
		/ sizeof(PassListener::commands[0]);

/**************************************************************
 * PassCmd
 **************************************************************/
void PassCmd::Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegate::SHADER_BLOCK) {
		String name;
		StringUtils::NextWord(ctx.GetParamList(), name);
		PassListener pass(static_cast<ShaderScript*>(parentParam), ctx);
		ctx.ParseBlock(&pass);
	} else {
		ctx.Error("BlendState block needs to be inside Shader declaration.");
	}
}

/**************************************************************
 * PassListener
 **************************************************************/
PassListener::PassListener(ShaderScript* _shader,
		const String& _name) : shaderScript(_shader), name(_name)  {
}

void PassListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	shaderScript->GetRequest()->AddPass(NamedObject::AsyncStringID(name));
	ctx.ParseStatements(this);
}

void PassListener::EnterStatement(ScriptParser::StatementContext& ctx) {
	CommandDelegate* cmd = Helper::FindCommand(PassListener::commands,
			PassListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd->Execute(CommandDelegate::PASS_BLOCK, shaderScript, ctx);
	// todo else throw error command not supported
}

} /* namespace ShaderCompiler */
