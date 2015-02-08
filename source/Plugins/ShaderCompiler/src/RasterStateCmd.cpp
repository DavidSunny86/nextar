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


CommandNamePair RasterStateListener::commands[] = {
	{ _SS(CMD_ANTI_ALIASING), &AntiAliasingCmd_Execute },
	{ _SS(CMD_DEPTH_BIAS), &DepthBiasCmd_Execute },
	{ _SS(CMD_DEPTH_CLIP), &DepthClipCmd_Execute },
	{ _SS(CMD_RASTER), &RasterCmd_Execute },
	{ _SS(CMD_SCISSOR), &ScissorCmd_Execute },
};

const size_t RasterStateListener::commandCount =
		sizeof(RasterStateListener::commands)
				/ sizeof(RasterStateListener::commands[0]);

void RasterStateListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void RasterStateListener::EnterStatement(ScriptParser::StatementContext& ctx) {
	CommandDelegate_Execute cmd = Helper::FindCommand(RasterStateListener::commands,
			RasterStateListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd(CommandDelegateBlock::RASTER_STATE_BLOCK, &this->state, ctx);
}

void RasterStateListener::RasterCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::RASTER_STATE_BLOCK);
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

void RasterStateListener::ScissorCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::RASTER_STATE_BLOCK);
	RasterState& rasterState = *(static_cast<RasterState*>(state));
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.usingScissors = Convert::ToBool(value);
	}
}

void RasterStateListener::AntiAliasingCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::RASTER_STATE_BLOCK);
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

void RasterStateListener::DepthClipCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::RASTER_STATE_BLOCK);
	RasterState& rasterState = *(static_cast<RasterState*>(state));
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		rasterState.depthClip = Convert::ToBool(value);
	}
}

void RasterStateListener::DepthBiasCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::RASTER_STATE_BLOCK);
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
