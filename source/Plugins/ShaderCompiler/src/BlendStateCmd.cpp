/*
 * BlendCmd.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include <EngineHeaders.h>
#include <CommonTypes.h>
#include <BlendStateCmd.h>
#include <ShaderAsset.h>
#include <ScriptStrings.h>
#include <ShaderScript.h>

namespace ShaderCompiler {


CommandNamePair BlendStateListener::commands[] = {
		{ _SS(CMD_BLEND), &BlendCmd_Execute },
		{ _SS(CMD_TARGET), &BlendTargetCmd_Execute },
};

const size_t BlendStateListener::commandCount =
		sizeof(BlendStateListener::commands)
				/ sizeof(BlendStateListener::commands[0]);


void BlendStateListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void BlendStateListener::EnterStatement(ScriptParser::StatementContext& ctx) {
	CommandDelegate_Execute cmd = Helper::FindCommand(BlendStateListener::commands,
			BlendStateListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd(CommandDelegateBlock::BLEND_STATE_BLOCK, &this->state, ctx);
}

void BlendStateListener::BlendCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::BLEND_STATE_BLOCK);
	BlendState& blendState = *(static_cast<BlendState*>(state));

	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	StringUtils::TokenIterator it = ConstMultiStringHelper::It(paramContext);
	if (it.HasNext(value)) {
		blendState.enabled = Convert::ToBool(value);
	}
	if (it.HasNext(value)) {
		blendState.alphaToCoverage = Convert::ToBool(value);
	}
	if (it.HasNext(value)) {
		blendState.numRenderTargets = (uint16) Convert::ToULong(value);
	}
}

void BlendStateListener::BlendTargetCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::BLEND_STATE_BLOCK);
	BlendState& blendState = *(static_cast<BlendState*>(state));
	uint32 targetIndex = 0;

	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	StringUtils::TokenIterator it = ConstMultiStringHelper::It(paramContext);
	if (it.HasNext(value)) {
		targetIndex = (uint32)Convert::ToULong(value);
		if (targetIndex >= blendState.numRenderTargets) {
			ctx.Error(
					"Target index is out of bounds. "
							"Please use Blend command to specify correct target index.");
			return;
		}
	}
	RenderTargetBlendOp& blendOp = blendState.blendOp[targetIndex];
	if (it.HasNext(value)) {
		blendOp.enabled = Convert::ToBool(value);
	}
	if (it.HasNext(value)) {
		blendOp.srcCol = Helper::GetBlendSource(value);
	}
	if (it.HasNext(value)) {
		blendOp.destCol = Helper::GetBlendSource(value);
	}
	if (it.HasNext(value)) {
		blendOp.colOp = Helper::GetColorOperation(value);
	}
	if (it.HasNext(value)) {
		blendOp.srcAlpha = Helper::GetBlendSource(value);
	}
	if (it.HasNext(value)) {
		blendOp.destAlpha = Helper::GetBlendSource(value);
	}
	if (it.HasNext(value)) {
		blendOp.alphaOp = Helper::GetColorOperation(value);
	}
	if (it.HasNext(value)) {
		blendOp.mask = Helper::GetColorMask(value);
	}
}
}

