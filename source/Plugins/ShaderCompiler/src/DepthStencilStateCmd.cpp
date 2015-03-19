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
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		depthStencil.depthTest = Convert::ToBool(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		depthStencil.depthWrite = Convert::ToBool(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		depthStencil.depthCompareFunc = Helper::GetDepthStencilFunc(value);
	}
}

void DepthStencilStateListener::StencilTestCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::DEPTH_STENCIL_BLOCK);
	DepthStencilState& depthStencil = *(static_cast<DepthStencilState*>(state));
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
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
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		faceOp->stencilFunc = Helper::GetDepthStencilFunc(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		faceOp->stencilFail = Helper::GetStencilOp(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		faceOp->stencilPass = Helper::GetStencilOp(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		faceOp->depthPass = Helper::GetStencilOp(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		faceOp->stencilMask = Convert::ToULong(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		faceOp->stencilRef = Convert::ToULong(value);
	}
}

} /* namespace nextar */
