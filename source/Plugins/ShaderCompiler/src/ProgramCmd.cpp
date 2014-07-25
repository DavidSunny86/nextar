/*
 * ProgramCmd.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include <BaseHeaders.h>
#include <ProgramCmd.h>
#include <ShaderAsset.h>
#include <ScriptStrings.h>
#include <ShaderScript.h>

namespace ShaderCompiler {

ProgramCmd ProgramCmd::command;
ParamCmd ParamCmd::command;
//AutoParamCmd AutoParamCmd::command;
ShaderCmd ShaderCmd::commandDomain(Pass::ProgramStage::STAGE_DOMAIN);
ShaderCmd ShaderCmd::commandVertex(Pass::ProgramStage::STAGE_VERTEX);
ShaderCmd ShaderCmd::commandFragment(Pass::ProgramStage::STAGE_FRAGMENT);
ShaderCmd ShaderCmd::commandHull(Pass::ProgramStage::STAGE_HULL);
ShaderCmd ShaderCmd::commandGeometry(Pass::ProgramStage::STAGE_GEOMETRY);

CommandNamePair ProgramListener::commands[] = {
//{ "AutoParam", &AutoParamCmd::command },
		{ _SS(CMD_DOMAIN_PROG), &ShaderCmd::commandDomain },
		{ _SS(CMD_FRAGMENT_PROG), &ShaderCmd::commandFragment },
		{ _SS(CMD_GEOMETRY_PROG), &ShaderCmd::commandGeometry },
		{ _SS(CMD_HULL_PROG), &ShaderCmd::commandHull },
		{ _SS(CMD_OPTION), &ParamCmd::command },
		{ _SS(CMD_PARAM), &ParamCmd::command },
		{ _SS(CMD_VERTEX_PROG), &ShaderCmd::commandVertex },
};

const size_t ProgramListener::commandCount = sizeof(ProgramListener::commands)
		/ sizeof(ProgramListener::commands[0]);

/**************************************************************
 * DepthStencil
 **************************************************************/
void ProgramCmd::Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& ctx) {
	if (parentType == CommandDelegate::PASS_BLOCK) {
		ProgramListener programListener(static_cast<ShaderScript*>(parentParam));
		ctx.ParseBlock(&programListener);
	} else {
		ctx.Error("Program block needs to be inside Shader declaration.");
	}
}

void ProgramListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void ProgramListener::EnterStatement(ScriptParser::StatementContext& ctx) {
	CommandDelegate* cmd = Helper::FindCommand(ProgramListener::commands,
			ProgramListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd->Execute(CommandDelegate::SHADER_BLOCK, shaderScript, ctx);
	// todo else throw error command not supported
}

void ParamCmd::Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegate::SHADER_BLOCK);
	ShaderTemplate::StreamRequest* shader =
			(static_cast<ShaderScript*>(state)->GetRequest());
	StringUtils::TokenIterator it = 0;
	String value;
	String name, param, desc;
	ParamDataType type = ParamDataType::PDT_UNKNOWN;
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		StringUtils::StringPair p = StringUtils::Split(value);
		if (p.second.length()) {
			type = ShaderAsset::MapParamType(p.first);
			param = p.second;
		} else
			param = p.first;
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		/** todo localization: should be a $variableName mapped to
		 * localization.zip/strings.table */
		name = value;
	}
	it = StringUtils::NextWord(paramContext, value, it);
	if (it != String::npos) {
		/** todo localization: should be a $variableName mapped to
		 * localization.zip/strings.table */
		desc = value;
	}

	if (!name.length())
		name = StringUtils::FormatName(param);
	if (ctx.GetCommand() == _SS(CMD_OPTION))
		shader->AddMacro(param, name, desc);
	else
		shader->AddParam(param, name, desc, type);
}

void ShaderCmd::Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement) {
	NEX_ASSERT(parentType == CommandDelegate::SHADER_BLOCK);
	ShaderScript* script = (static_cast<ShaderScript*>(parentParam));
	String programRegion;
	StringUtils::TokenIterator it = StringUtils::NextWord(statement.GetParamList(),
			programRegion);
	script->SetRegionAsSource(stage, programRegion);
}

} /* namespace ShaderCompiler */

