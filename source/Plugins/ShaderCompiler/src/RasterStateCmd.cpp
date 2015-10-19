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
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	StringUtils::TokenIterator it = ConstMultiStringHelper::It(paramContext);
	if (it.HasNext(value)) {
		rasterState.fill = Helper::GetFillMode(value);
	}
	if (it.HasNext(value)) {
		rasterState.cull = Helper::GetCullMode(value);
	}
	if (it.HasNext(value)) {
		rasterState.trianglesAreClockwise = Convert::ToBool(value);
	}
}

void RasterStateListener::ScissorCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::RASTER_STATE_BLOCK);
	RasterState& rasterState = *(static_cast<RasterState*>(state));
	ConstMultiStringHelper h(ctx.GetParamList());
	String value = h.Get(0);
	if (value.length()) {
		rasterState.usingScissors = Convert::ToBool(value);
	}
}

void RasterStateListener::AntiAliasingCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::RASTER_STATE_BLOCK);
	RasterState& rasterState = *(static_cast<RasterState*>(state));
	String value;
	ConstMultiStringHelper::Iterator it = ConstMultiStringHelper::It(ctx.GetParamList());
	if (it.HasNext(value)) {
		rasterState.usingMultisample = Convert::ToBool(value);
	}
	if (it.HasNext(value)) {
		rasterState.usingLineAa = Convert::ToBool(value);
	}
}

void RasterStateListener::DepthClipCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::RASTER_STATE_BLOCK);
	RasterState& rasterState = *(static_cast<RasterState*>(state));
	String value;
	ConstMultiStringHelper::Iterator it = ConstMultiStringHelper::It(ctx.GetParamList());
	if (it.HasNext(value)) {
		rasterState.depthClip = Convert::ToBool(value);
	}
}

void RasterStateListener::DepthBiasCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::RASTER_STATE_BLOCK);
	RasterState& rasterState = *(static_cast<RasterState*>(state));

	String value;
	ConstMultiStringHelper::Iterator it = ConstMultiStringHelper::It(ctx.GetParamList());
	if (it.HasNext(value)) {
		rasterState.constantDepthBias = Convert::ToFloat(value);
	}
	if (it.HasNext(value)) {
		rasterState.slopeScaledDepthBias = Convert::ToFloat(value);
	}
	if (it.HasNext(value)) {
		rasterState.depthBiasClamp = Convert::ToFloat(value);
	}
}
}
