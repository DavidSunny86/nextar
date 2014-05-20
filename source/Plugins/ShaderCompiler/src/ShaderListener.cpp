/*
 * ShaderCompiler.cpp
 *
 *  Created on: 22-Sep-2013
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

namespace ShaderCompiler {

	/**************************************************************
	 * Command Map
	 **************************************************************/
	CommandNamePair ShaderListener::commands[] = {
		{ _SS(CMD_BLEND_STATE), &BlendStateCmd::command },
		{ _SS(CMD_DEPTH_STENCIL_STATE), &DepthStencilStateCmd::command },
		{ _SS(CMD_RASTER_STATE), &RasterStateCmd::command },
		{ _SS(CMD_TEXTURE_STATE), &TextureUnitStateCmd::command },
	};

	const size_t ShaderListener::commandCount =
			sizeof(ShaderListener::commands)/sizeof(ShaderListener::commands[0]);

	/**************************************************************
	 * ShaderListener
	 **************************************************************/
	ShaderListener::ShaderListener(ShaderScript* sh, const String& _name) : shaderScript(sh), name(_name) {
	}

	void ShaderListener::EnterBlock(ScriptParser::BlockContext& ctx) {
		ctx.ParseStatements(this);
	}

	void ShaderListener::EnterStatement(ScriptParser::StatementContext& ctx) {
		CommandDelegate* cmd = Helper::FindCommand(ShaderListener::commands,
				ShaderListener::commandCount, ctx.command);
		if (cmd)
			cmd->Execute(CommandDelegate::SHADER_BLOCK, shaderScript, ctx);
		// todo else throw error command not supported
	}

}
