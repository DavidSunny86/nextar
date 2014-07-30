/*
 * RasterCmd.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include <NexEngine.h>
#include <RasterState.h>
#include <RasterStateCmd.h>
#include <ShaderAsset.h>
#include <ScriptStrings.h>
#include <ShaderScript.h>

namespace ShaderCompiler {

RasterStateCmd RasterStateCmd::command;
AntiAliasingCmd AntiAliasingCmd::command;
DepthBiasCmd DepthBiasCmd::command;
DepthClipCmd DepthClipCmd::command;
RasterCmd RasterCmd::command;
ScissorCmd ScissorCmd::command;

CommandNamePair RasterStateListener::commands[] = {
		{ _SS(CMD_ANTI_ALIASING), &AntiAliasingCmd::command },
		{ _SS(CMD_DEPTH_BIAS), &DepthBiasCmd::command },
		{ _SS(CMD_DEPTH_CLIP), &DepthClipCmd::command },
		{ _SS(CMD_RASTER), &RasterCmd::command },
		{ _SS(CMD_SCISSOR), &ScissorCmd::command },
};

const size_t RasterStateListener::commandCount =
		sizeof(RasterStateListener::commands)
				/ sizeof(RasterStateListener::commands[0]);
/**************************************************************
 * RasterState
 ***************************************************************/
void RasterStateCmd::Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegate::PASS_BLOCK) {
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

void RasterStateListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void RasterStateListener::EnterStatement(ScriptParser::StatementContext& ctx) {
	CommandDelegate* cmd = Helper::FindCommand(RasterStateListener::commands,
			RasterStateListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd->Execute(CommandDelegate::RASTER_STATE_BLOCK, &this->state, ctx);
}

void RasterCmd::Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegate::RASTER_STATE_BLOCK);
	RasterState& rasterState = *(static_cast<RasterState*>(state));
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.fill = Helper::GetFillMode(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.cull = Helper::GetCullMode(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.trianglesAreClockwise = Convert::ToBool(value);
	}
}

void ScissorCmd::Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegate::RASTER_STATE_BLOCK);
	RasterState& rasterState = *(static_cast<RasterState*>(state));
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.usingScissors = Convert::ToBool(value);
	}
}

void AntiAliasingCmd::Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegate::RASTER_STATE_BLOCK);
	RasterState& rasterState = *(static_cast<RasterState*>(state));
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.usingMultisample = Convert::ToBool(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.usingLineAa = Convert::ToBool(value);
	}
}

void DepthClipCmd::Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegate::RASTER_STATE_BLOCK);
	RasterState& rasterState = *(static_cast<RasterState*>(state));
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.depthClip = Convert::ToBool(value);
	}
}

void DepthBiasCmd::Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegate::RASTER_STATE_BLOCK);
	RasterState& rasterState = *(static_cast<RasterState*>(state));
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.constantDepthBias = Convert::ToFloat(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.slopeScaledDepthBias = Convert::ToFloat(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.depthBiasClamp = Convert::ToFloat(value);
	}
}
}
