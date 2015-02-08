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

/**************************************************************
 * Command Map
 **************************************************************/
CommandNamePair PassListener::commands[] = {
		{ _SS(CMD_BLEND_STATE), &BlendStateCmd_Execute },
		{ _SS(CMD_CBUFFER), &ConstBufferCmd_Execute },
		{ _SS(CMD_DEPTH_STENCIL_STATE), &DepthStencilStateCmd_Execute },
		{ _SS(CMD_PROGRAM), &ProgramCmd_Execute },
		{ _SS(CMD_RASTER_STATE), &RasterStateCmd_Execute },
		{ _SS(CMD_TEXTURE_STATE), &TextureUnitStateCmd_Execute },
};

const size_t PassListener::commandCount = sizeof(PassListener::commands)
		/ sizeof(PassListener::commands[0]);

/**************************************************************
* BlendState
***************************************************************/
void PassListener::BlendStateCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegateBlock::PASS_BLOCK) {
		BlendStateListener blend;
		ShaderTemplate::LoadStreamRequest* shader =
			static_cast<ShaderScript*>(parentParam)->GetRequest();
		ctx.ParseBlock(&blend);
		if (!ctx.IsErrorBitSet()) {
			shader->SetBlendState(blend.state);
		}
	} else {
		ctx.Error("BlendState block needs to be inside Shader declaration.");
	}
}


/**************************************************************
* ConstBuffer
**************************************************************/
void PassListener::ConstBufferCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {

	ShaderScript* script = static_cast<ShaderScript*>(parentParam);
	String name;
	StringUtils::NextWord(ctx.GetParamList(), name);

	InputStreamPtr file = script->FetchConstBuffer(name);

	if (file) {
		script->GetTranslator().TranslateConstantBuffer(script, name, file);
	} else {
		ctx.Error("Could not load cbuffer: " + name);
	}
}

/**************************************************************
* DepthStencil
**************************************************************/
void PassListener::DepthStencilStateCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegateBlock::PASS_BLOCK) {
		DepthStencilStateListener depthStencil;
		ShaderTemplate::LoadStreamRequest* shader =
			static_cast<ShaderScript*>(parentParam)->GetRequest();
		ctx.ParseBlock(&depthStencil);
		if (!ctx.IsErrorBitSet()) {
			shader->SetDepthStencilState(depthStencil.state);
		}
	} else {
		ctx.Error(
			"DepthStencilState block needs to be inside Shader declaration.");
	}
}

/**************************************************************
* Program
**************************************************************/
void PassListener::ProgramCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegateBlock::PASS_BLOCK) {
		ProgramListener programListener(reinterpret_cast<ShaderScript*>(parentParam));
		ctx.ParseBlock(&programListener);
	} else {
		ctx.Error("Program block needs to be inside Shader declaration.");
	}
}

/**************************************************************
* RasterState
***************************************************************/
void PassListener::RasterStateCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegateBlock::PASS_BLOCK) {
		RasterStateListener raster;
		ShaderTemplate::LoadStreamRequest* shader =
			static_cast<ShaderScript*>(parentParam)->GetRequest();
		ctx.ParseBlock(&raster);
		if (!ctx.IsErrorBitSet()) {
			shader->SetRasterState(raster.state);
		}
	} else {
		ctx.Error("RasterState block needs to be inside Shader declaration.");
	}
}

/**************************************************************
* TextureUnitState
**************************************************************/
void PassListener::TextureUnitStateCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegateBlock::PASS_BLOCK) {
		ShaderScript* script = reinterpret_cast<ShaderScript*>(parentParam);
		TextureUnitStateListener textureUnitState(script->GetRequest());
		ctx.ParseBlock(&textureUnitState);
	} else {
		ctx.Error("TextureState block needs to be inside Shader declaration.");
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
	CommandDelegate_Execute cmd = Helper::FindCommand(PassListener::commands,
			PassListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd(CommandDelegateBlock::PASS_BLOCK, shaderScript, ctx);
	else
		ctx.Error("Command not supported: " + ctx.GetCommand());
}

} /* namespace ShaderCompiler */
