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


CommandNamePair ProgramListener::commands[] = {
	//{ "AutoParam", &AutoParamCmd::command },
	{ _SS(CMD_DEFINE), &ParamCmd_Execute },
	{ _SS(CMD_DOMAIN_PROG), &DomainCmd_Execute },
	{ _SS(CMD_FRAGMENT_PROG), &FragmentCmd_Execute },
	{ _SS(CMD_GEOMETRY_PROG), &GeometryCmd_Execute },
	{ _SS(CMD_HULL_PROG), &HullCmd_Execute },
	{ _SS(CMD_OPTION), &ParamCmd_Execute },
	{ _SS(CMD_VERTEX_PROG), &VertexCmd_Execute },
};

const size_t ProgramListener::commandCount = sizeof(ProgramListener::commands)
		/ sizeof(ProgramListener::commands[0]);


void ProgramListener::EnterBlock(ScriptParser::BlockContext& ctx) {
	ctx.ParseStatements(this);
}

void ProgramListener::EnterStatement(ScriptParser::StatementContext& ctx) {
	CommandDelegate_Execute cmd = Helper::FindCommand(ProgramListener::commands,
			ProgramListener::commandCount, ctx.GetCommand());
	if (cmd)
		cmd(CommandDelegateBlock::SHADER_BLOCK, shaderScript, ctx);
	// todo else throw error command not supported
}

void ProgramListener::DomainCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::SHADER_BLOCK);
	ShaderCmd_Execute((static_cast<ShaderScript*>(parentParam)), ctx, Pass::ProgramStage::STAGE_DOMAIN);
}

void ProgramListener::GeometryCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::SHADER_BLOCK);
	ShaderCmd_Execute((static_cast<ShaderScript*>(parentParam)), ctx, Pass::ProgramStage::STAGE_GEOMETRY);
}

void ProgramListener::HullCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::SHADER_BLOCK);
	ShaderCmd_Execute((static_cast<ShaderScript*>(parentParam)), ctx, Pass::ProgramStage::STAGE_HULL);
}

void ProgramListener::FragmentCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::SHADER_BLOCK);
	ShaderCmd_Execute((static_cast<ShaderScript*>(parentParam)), ctx, Pass::ProgramStage::STAGE_FRAGMENT);
}

void ProgramListener::VertexCmd_Execute(int parentType, void* parentParam,
	ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::SHADER_BLOCK);
	ShaderCmd_Execute((static_cast<ShaderScript*>(parentParam)), ctx, Pass::ProgramStage::STAGE_VERTEX);
}

void ProgramListener::ParamCmd_Execute(int parentType, void* state,
		ScriptParser::StatementContext& ctx) {
	NEX_ASSERT(parentType == CommandDelegateBlock::SHADER_BLOCK);
	ShaderScript* script = reinterpret_cast<ShaderScript*>(state);
	ShaderTemplate::LoadStreamRequest* shader =
			(script->GetRequest());
	String value;
	String name, uiName, desc;
	
	const StringUtils::WordList& paramContext = ctx.GetParamList();
	StringUtils::TokenIterator it = ConstMultiStringHelper::It(paramContext);
	it(name);
	uiName = ctx.GetTaggedParamVal(_SS(TAG_UI), it);
	desc = ctx.GetTaggedParamVal(_SS(TAG_DESC), it);

	if (ctx.GetCommand() == _SS(CMD_OPTION))
		shader->AddMacro(name, uiName, desc);
	else {
		// per-define it
		script->GetTranslator().AddMacro(script, name);
	}
}

void ProgramListener::ShaderCmd_Execute(ShaderScript* script,
	ScriptParser::StatementContext& statement, Pass::ProgramStage stage) {
	
	String programName;
	const StringUtils::WordList& words = statement.GetParamList();
	StringUtils::TokenIterator prev = ConstMultiStringHelper::It(words);
	while (	prev.HasNext(programName) )  {
		for(int i = 0; i < RenderManager::ShaderLanguage::SPP_COUNT; ++i) {
			InputStreamPtr program =
					script->FetchProgram(programName, (RenderManager::ShaderLanguage)i, stage);
			if (program) {

				const void* buffer = nullptr;
				size_t num = program->AcquireBuffer(buffer);
				String programString;
				programString.reserve(num + 1);
				programString.assign((const char*)buffer, num);
				script->AddRegion(statement.GetCommand(), (RenderManager::ShaderLanguage)i,
						std::move(programString));

				program->ReleaseBuffer(buffer);
			}

		}
	}
	script->SetStageActive(stage, true);
	//script->SetRegionAsSource(stage, programRegion);
}

} /* namespace ShaderCompiler */

