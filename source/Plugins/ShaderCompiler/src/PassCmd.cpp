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
#include <ConstBufferCmd.h>

namespace ShaderCompiler {

PassCmd PassCmd::command;

/**************************************************************
 * Command Map
 **************************************************************/
CommandNamePair PassListener::commands[] = {
		{ _SS(CMD_BLEND_STATE), &BlendStateCmd::command },
		{ _SS(CMD_CBUFFER), &ConstBufferCmd::command },
		{ _SS(CMD_DEPTH_STENCIL_STATE), &DepthStencilStateCmd::command },
		{ _SS(CMD_PROGRAM), &ProgramCmd::command },
		{ _SS(CMD_RASTER_STATE), &RasterStateCmd::command },
		{ _SS(CMD_TEXTURE_STATE), &TextureUnitStateCmd::command },
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
		auto shaderScript = static_cast<ShaderScript*>(parentParam);
		PassListener pass(shaderScript, name);
		ctx.ParseBlock(&pass);
		// create sources from regions
		CreateSourcesFromRegions(shaderScript);
	} else {
		ctx.Error("BlendState block needs to be inside Shader declaration.");
	}
}

void PassCmd::CreateSourcesFromRegions(ShaderScript* shaderScript) {
	String cbuffer = _SS(CMD_CBUFFER);
	for(uint32 i = 0; i < Pass::STAGE_COUNT; ++i) {
		StringUtils::WordList words;
		String src;

		switch((Pass::ProgramStage)i) {
		case Pass::ProgramStage::STAGE_DOMAIN:
			src = _SS(CMD_DOMAIN_PROG); break;
		case Pass::ProgramStage::STAGE_VERTEX:
			src = _SS(CMD_VERTEX_PROG); break;
		case Pass::ProgramStage::STAGE_FRAGMENT:
			src = _SS(CMD_FRAGMENT_PROG); break;
		case Pass::ProgramStage::STAGE_HULL:
			src = _SS(CMD_HULL_PROG); break;
		case Pass::ProgramStage::STAGE_GEOMETRY:
			src = _SS(CMD_GEOMETRY_PROG); break;

		}
		StringUtils::PushBackWord(words, cbuffer);
		StringUtils::PushBackWord(words, src);
		shaderScript->SetRegionsAsSource(Pass::STAGE_VERTEX, words);
		shaderScript->RemoveRegion(src);
	}

	shaderScript->RemoveRegion(cbuffer);
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
	else
		ctx.Error("Command not supported: " + ctx.GetCommand());
}

} /* namespace ShaderCompiler */
