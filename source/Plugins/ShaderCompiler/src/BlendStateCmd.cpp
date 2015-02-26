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
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		blendState.enabled = Convert::ToBool(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		blendState.alphaToCoverage = Convert::ToBool(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		blendState.numRenderTargets = (uint16) Convert::ToULong(value);
	}
}

void BlendStateListener::BlendTargetCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::BLEND_STATE_BLOCK);
	BlendState& blendState = *(static_cast<BlendState*>(state));
	uint32 targetIndex = 0;
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		targetIndex = (uint32)Convert::ToULong(value);
		if (targetIndex >= blendState.numRenderTargets) {
			ctx.Error(
					"Target index is out of bounds. "
							"Please use Blend command to specify correct target index.");
			return;
		}
	}
	RenderTargetBlendOp& blendOp = blendState.blendOp[targetIndex];
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		blendOp.enabled = Convert::ToBool(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		blendOp.srcCol = Helper::GetBlendSource(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		blendOp.destCol = Helper::GetBlendSource(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		blendOp.colOp = Helper::GetColorOperation(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		blendOp.srcAlpha = Helper::GetBlendSource(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		blendOp.destAlpha = Helper::GetBlendSource(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		blendOp.alphaOp = Helper::GetColorOperation(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		blendOp.mask = Helper::GetColorMask(value);
	}
}
}

