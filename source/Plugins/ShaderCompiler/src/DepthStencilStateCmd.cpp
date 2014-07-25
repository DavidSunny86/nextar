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

DepthStencilStateCmd DepthStencilStateCmd::command;
DepthStateCmd DepthStateCmd::command;
StencilTestCmd StencilTestCmd::command;
StencilOpCmd StencilOpCmd::command;

CommandNamePair DepthStencilStateListener::commands[] = {
		{ _SS(CMD_DEPTH_STATE), &DepthStateCmd::command },
		{ _SS(CMD_STENCIL_BACK_OP), &StencilOpCmd::command },
		{ _SS(CMD_STENCIL_FRONT_OP), &StencilOpCmd::command },
		{ _SS(CMD_STENCIL_TEST), &StencilTestCmd::command },
};

const size_t DepthStencilStateListener::commandCount =
		sizeof(DepthStencilStateListener::commands)
				/ sizeof(DepthStencilStateListener::commands[0]);

/**************************************************************
 * DepthStencil
 **************************************************************/
void DepthStencilStateCmd::Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegate::PASS_BLOCK) {
		DepthStencilStateListener depthStencil;
		ShaderTemplate::StreamRequest* shader =
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

void DepthStencilStateListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void DepthStencilStateListener::EnterStatement(
		ScriptParser::StatementContext& ctx) {
	CommandDelegate* cmd = Helper::FindCommand(
			DepthStencilStateListener::commands,
			DepthStencilStateListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd->Execute(CommandDelegate::DEPTH_STENCIL_BLOCK, &this->state, ctx);
	// todo else throw error command not supported
}

void DepthStateCmd::Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegate::DEPTH_STENCIL_BLOCK);
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

void StencilTestCmd::Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegate::DEPTH_STENCIL_BLOCK);
	DepthStencilState& depthStencil = *(static_cast<DepthStencilState*>(state));
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		depthStencil.stencilTest = Convert::ToBool(value);
	}
}

void StencilOpCmd::Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegate::DEPTH_STENCIL_BLOCK);
	DepthStencilState& depthStencil = *(static_cast<DepthStencilState*>(state));
	StencilFaceOp* faceOp =
			ctx.GetCommand() == "StencilFrontOp" ?
					&depthStencil.front : &depthStencil.back;
	StringUtils::TokenIterator it = 0;
	String value;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		faceOp->stencilMask = Helper::GetColorMask(value);
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		faceOp->stencilFunc = Helper::GetComparisonFunc(value);
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
}

} /* namespace nextar */
