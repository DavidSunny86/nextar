/*
 * BlendCmd.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include <NexEngine.h>
#include <CommonTypes.h>
#include <BlendStateCmd.h>
#include <Shader.h>

namespace ShaderCompiler {

	BlendStateCmd BlendStateCmd::command;
	BlendCmd BlendCmd::command;
	BlendTargetCmd BlendTargetCmd::command;

	CommandNamePair BlendStateListener::commands[] = {
		{ "Blend", &BlendCmd::command },
		{ "Target", &BlendTargetCmd::command },
	};

	const size_t BlendStateListener::commandCount =
			sizeof(BlendStateListener::commands)/sizeof(BlendStateListener::commands[0]);

	/**************************************************************
	 * BlendState
	 ***************************************************************/
	void BlendStateCmd::Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& ctx) {
		if (parentType == CommandDelegate::SHADER_BLOCK) {
			BlendStateListener blend;
			ShaderAsset::StreamRequest* shader = static_cast<ShaderAsset::StreamRequest*>(parentParam);
			ctx.ParseBlock(&blend);
			if (!ctx.IsErrorBitSet()) {
				shader->SetBlendState(blend.state);
			}
		} else {
			ctx.Error("BlendState block needs to be inside Shader declaration.");
		}
	}

	void BlendStateListener::EnterBlock(ScriptParser::BlockContext& ctx) {
		ctx.ParseStatements(this);
	}

	void BlendStateListener::EnterStatement(ScriptParser::StatementContext& ctx) {
		CommandDelegate* cmd = Helper::FindCommand(BlendStateListener::commands,
			BlendStateListener::commandCount, ctx.GetCommand());
		if (cmd)
			cmd->Execute(CommandDelegate::BLEND_STATE_BLOCK, &this->state, ctx);
	}

	void BlendCmd::Execute(int parentType, void* state,
			ScriptParser::StatementContext& ctx) {
		NEX_ASSERT (parentType == CommandDelegate::BLEND_STATE_BLOCK);
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
			blendState.numRenderTargets = (uint16)Convert::ToULong(value);
		}
	}

	void BlendTargetCmd::Execute(int parentType, void* state,
				ScriptParser::StatementContext& ctx) {
		NEX_ASSERT (parentType == CommandDelegate::BLEND_STATE_BLOCK);
		BlendState& blendState = *(static_cast<BlendState*>(state));
		uint32 targetIndex = 0;
		StringUtils::TokenIterator it = 0;
		String value;
		const StringUtils::WordList& paramContext = ctx.GetParamList();
		it = StringUtils::NextWord(paramContext, value, it);
		if (it != String::npos) {
			targetIndex = Convert::ToULong(value);
			if (targetIndex >= blendState.numRenderTargets) {
				ctx.Error("Target index is out of bounds. "
						"Please use Blend command to specify correct target index.");
			}
		}
		RenderTargetBlendOp& blendOp = blendState.blendOp[targetIndex];
		it = StringUtils::NextWord(paramContext, value, it);
		if (it != String::npos) {
			blendOp.enabled = Convert::ToBool(value);
		}
		it = StringUtils::NextWord(paramContext, value, it);
		if (it != String::npos) {
			blendOp.mask = Helper::GetColorMask(value);
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
	}
}

