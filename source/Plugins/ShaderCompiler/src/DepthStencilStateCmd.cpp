/*
 * DepthStencil.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include <BaseHeaders.h>
#include <CommonTypes.h>
#include <DepthStencilStateCmd.h>
#include <ShaderAsset.h>
#include <ScriptStrings.h>
#include <ShaderScript.h>

namespace ShaderCompiler {

CommandNamePair DepthStencilStateListener::commands[] = {
		{ _SS(CMD_DEPTH_STATE), &DepthStateCmd_Execute },
		{ _SS(CMD_STENCIL_BACK_OP), &StencilOpCmd_Execute },
		{ _SS(CMD_STENCIL_FRONT_OP), &StencilOpCmd_Execute },
		{ _SS(CMD_STENCIL_TEST), &StencilTestCmd_Execute },
};

const size_t DepthStencilStateListener::commandCount =
		sizeof(DepthStencilStateListener::commands)
				/ sizeof(DepthStencilStateListener::commands[0]);


void DepthStencilStateListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void DepthStencilStateListener::EnterStatement(
		ScriptParser::StatementContext& ctx) {
	CommandDelegate_Execute cmd = Helper::FindCommand(
			DepthStencilStateListener::commands,
			DepthStencilStateListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd(CommandDelegateBlock::DEPTH_STENCIL_BLOCK, &this->state, ctx);
	// todo else throw error command not supported
}

void DepthStencilStateListener::DepthStateCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::DEPTH_STENCIL_BLOCK);
	DepthStencilState& depthStencil = *(static_cast<DepthStencilState*>(state));

	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	StringUtils::TokenIterator it = ConstMultiStringHelper::It(paramContext);
	if (it.HasNext(value)) {
		depthStencil.depthTest = Convert::ToBool(value);
	}
	if (it.HasNext(value)) {
		depthStencil.depthWrite = Convert::ToBool(value);
	}
	if (it.HasNext(value)) {
		depthStencil.depthCompareFunc = Helper::GetDepthStencilFunc(value);
	}
}

void DepthStencilStateListener::StencilTestCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::DEPTH_STENCIL_BLOCK);
	DepthStencilState& depthStencil = *(static_cast<DepthStencilState*>(state));
	String value = ConstMultiStringHelper(ctx.GetParamList()).Get(0);
	if (value.length()) {
		depthStencil.stencilTest = Convert::ToBool(value);
	}
}

void DepthStencilStateListener::StencilOpCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::DEPTH_STENCIL_BLOCK);
	DepthStencilState& depthStencil = *(static_cast<DepthStencilState*>(state));
	StencilFaceOp* faceOp =
			ctx.GetCommand() == "StencilFrontOp" ?
					&depthStencil.front : &depthStencil.back;
	StringUtils::TokenIterator it = ConstMultiStringHelper::It(ctx.GetParamList());
	String value;
	if (it.HasNext(value)) {
		faceOp->stencilFunc = Helper::GetDepthStencilFunc(value);
	}
	if (it.HasNext(value)) {
		faceOp->stencilFail = Helper::GetStencilOp(value);
	}
	if (it.HasNext(value)) {
		faceOp->stencilPass = Helper::GetStencilOp(value);
	}
	if (it.HasNext(value)) {
		faceOp->depthPass = Helper::GetStencilOp(value);
	}
	if (it.HasNext(value)) {
		faceOp->stencilMask = Convert::ToULong(value);
	}
	if (it.HasNext(value)) {
		faceOp->stencilRef = Convert::ToULong(value);
	}
}

} /* namespace nextar */
