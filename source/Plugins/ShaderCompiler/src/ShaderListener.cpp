/*
 * ShaderCompiler.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <ShaderListener.h>
#include <PassCmd.h>
#include <ShaderScript.h>
#include <ScriptStrings.h>

namespace ShaderCompiler {

/**************************************************************
 * Command Map
 **************************************************************/
CommandNamePair ShaderListener::commands[] = {
		{ _SS(CMD_FLAGS), &FlagsCmd_Execute },
		{ _SS(CMD_PASS), &PassCmd_Execute },
};

const size_t ShaderListener::commandCount = sizeof(ShaderListener::commands)
		/ sizeof(ShaderListener::commands[0]);

/**************************************************************
 * ShaderListener
 **************************************************************/
ShaderListener::ShaderListener(ShaderScript* sh, const String& _name) :
		shaderScript(sh), name(_name) {
}

void ShaderListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void ShaderListener::EnterStatement(ScriptParser::StatementContext& ctx) {
	CommandDelegate_Execute cmd = Helper::FindCommand(ShaderListener::commands,
			ShaderListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd(CommandDelegateBlock::SHADER_BLOCK, shaderScript, ctx);
	// todo else throw error command not supported
}

/**************************************************************
* PassCmd
**************************************************************/
void ShaderListener::PassCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegateBlock::SHADER_BLOCK) {
		String name;
		ConstMultiStringHelper h(ctx.GetParamList());
		name = h.Get(0);
		auto shaderScript = static_cast<ShaderScript*>(parentParam);
		PassListener pass(shaderScript, name);
		ctx.ParseBlock(&pass);
		// create sources from regions
		CreateSourcesFromRegions(shaderScript);
	} else {
		ctx.Error("BlendState block needs to be inside Shader declaration.");
	}
}

/**************************************************************
 * FlagsCmd
 **************************************************************/
void ShaderListener::FlagsCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegateBlock::SHADER_BLOCK) {
		const StringUtils::WordList& wl = ctx.GetParamList();
		String value;
		StringUtils::TokenIterator prev = ConstMultiStringHelper::It(wl);
		uint32 flags = 0;
		while (	(prev = StringUtils::NextWord(wl, value, prev)) != String::npos ) {
			flags |= Helper::GetShaderFlag(value);
		}

		if (flags) {
			ShaderTemplate* shader = static_cast<ShaderTemplate*>(
					static_cast<ShaderScript*>(parentParam)->GetRequest()->GetStreamedObject());
			shader->SetRenderFlags(flags);
		}
	} else {
		ctx.Error("BlendState block needs to be inside Shader declaration.");
	}
}

void ShaderListener::CreateSourcesFromRegions(ShaderScript* shaderScript) {
	String predefs = _SS(REG_PREDEFS);
	String defines = _SS(REG_DEFINE);
	String cbuffer = _SS(REG_CBUFFER);
	
	for (uint32 i = 0; i < Pass::STAGE_COUNT; ++i) {
		StringUtils::WordList words;
		String src;

		switch ((Pass::ProgramStage)i) {
		case Pass::ProgramStage::STAGE_DOMAIN:
			src = _SS(CMD_DOMAIN_PROG); break;
		case Pass::ProgramStage::STAGE_VERTEX:
			src = _SS(CMD_VERTEX_PROG); break;
		case Pass::ProgramStage::STAGE_FRAGMENT:
			src = _SS(CMD_FRAGMENT_PROG); break;
		case Pass::ProgramStage::STAGE_HULL:
			src = _SS(CMD_HULL_PROG); break;
		case Pass::ProgramStage::STAGE_GEOMETRY:
			src = _SS(CMD_GEOMETRY_PROG); break;

		}
		if (shaderScript->IsStageActive((Pass::ProgramStage)i)) {
			shaderScript->GetTranslator().AddPredefs((Pass::ProgramStage)i, shaderScript);
			StringUtils::PushBackWord(words, predefs);
			StringUtils::PushBackWord(words, defines);
			StringUtils::PushBackWord(words, cbuffer);
			StringUtils::PushBackWord(words, src);
			shaderScript->SetRegionsAsSource((Pass::ProgramStage)i, words);
			shaderScript->RemoveRegion(src);
		}
	}

	shaderScript->RemoveRegion(cbuffer);
}

}
