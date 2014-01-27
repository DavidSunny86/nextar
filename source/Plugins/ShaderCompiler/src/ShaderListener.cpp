/*
 * ShaderCompiler.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "ShaderListener.h"
#include "DepthStencilStateCmd.h"
#include "BlendStateCmd.h"
#include "RasterStateCmd.h"
#include "TextureUnitStateCmd.h"

namespace ShaderCompiler {

	/**************************************************************
	 * Command Map
	 **************************************************************/
	CommandNamePair ShaderListener::commands[] = {
		{ "BlendState", &BlendStateCmd::command },
		{ "DepthStencilState", &DepthStencilStateCmd::command },
		{ "RasterState", &RasterStateCmd::command },
		{ "TextureState", &TextureUnitStateCmd::command },
	};

	const size_t ShaderListener::commandCount =
			sizeof(ShaderListener::commands)/sizeof(ShaderListener::commands[0]);

	/**************************************************************
	 * ShaderListener
	 **************************************************************/
	ShaderListener::ShaderListener(Shader::StreamRequest* sh, const String& _name) : shader(sh), name(_name) {
	}

	void ShaderListener::EnterBlock(ScriptParser::BlockContext& ctx) {
		ctx.ParseStatements(this);
	}

	void ShaderListener::EnterStatement(ScriptParser::StatementContext& ctx) {
		CommandDelegate* cmd = Helper::FindCommand(ShaderListener::commands,
				ShaderListener::commandCount, ctx.command);
		if (cmd)
			cmd->Execute(CommandDelegate::SHADER_BLOCK, shader, ctx);
		// todo else throw error command not supported
	}

}
