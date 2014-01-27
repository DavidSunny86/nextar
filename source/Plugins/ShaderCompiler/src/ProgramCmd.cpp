/*
 * ProgramCmd.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include "NexHeaders.h"
#include "ProgramCmd.h"
#include "Shader.h"

namespace ShaderCompiler {

	ProgramCmd ProgramCmd::command;
	ParamCmd ParamCmd::command;
	//AutoParamCmd AutoParamCmd::command;
	ShaderCmd ShaderCmd::command;

	CommandNamePair ProgramListener::commands[] = {
		//{ "AutoParam", &AutoParamCmd::command },
		{ "FragmentProgram", &ShaderCmd::command },
		{ "GeometryProgram", &ShaderCmd::command },
		{ "Option", &ParamCmd::command },
		{ "Param", &ParamCmd::command },
		{ "VertexProgram", &ShaderCmd::command },
	};

	const size_t ProgramListener::commandCount =
				sizeof(ProgramListener::commands)/sizeof(ProgramListener::commands[0]);

	/**************************************************************
	 * DepthStencil
	 **************************************************************/
	void ProgramCmd::Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& ctx) {
		if (parentType == CommandDelegate::SHADER_BLOCK) {
			Shader::StreamRequest* shader = static_cast<Shader::StreamRequest*>(parentParam);
			ProgramListener programListener(shader);
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
			cmd->Execute(CommandDelegate::SHADER_BLOCK, shader, ctx);
		// todo else throw error command not supported
	}

	void ParamCmd::Execute(int parentType, void* state,
			ScriptParser::StatementContext& ctx) {
		NEX_ASSERT (parentType == CommandDelegate::SHADER_BLOCK);
		Shader::StreamRequest* shader = (static_cast<Shader::StreamRequest*>(state));
		StringUtils::TokenIterator it = 0;
		String value;
		String name, param, defaultVal, desc;
		ParamDataType type = ParamDataType::PDT_UNKNOWN;
		const StringUtils::WordList& paramContext = ctx.GetParamList();
		it = StringUtils::NextWord(paramContext, value, it);
		if (it != String::npos) {
			StringUtils::StringPair p = StringUtils::Split(value);
			if (p.second.length()) {
				type = Shader::MapParamType(value);
				param = p.second;
			} else
				param = p.first;
		}
		it = StringUtils::NextWord(paramContext, value, it);
		if (it != String::npos) {
			/** @todo localization: should be a $variableName mapped to
			 * localization.zip/strings.table */
			name = value;
		}
		it = StringUtils::NextWord(paramContext, value, it);
		if (it != String::npos) {
			defaultVal = value;
		}
		it = StringUtils::NextWord(paramContext, value, it);
		if (it != String::npos) {
			/** @todo localization: should be a $variableName mapped to
			 * localization.zip/strings.table */
			desc = value;
		}
		it = StringUtils::NextWord(paramContext, value, it);

		if (!name.length())
			name = StringUtils::FormatName(param);
		if (ctx.GetCommand() == "Option")
			shader->AddMacro(name, param, desc, Convert::ToBool(defaultVal));
		else
			shader->AddParam(name, param, desc, defaultVal, type);
	}
		
	void ShaderCmd::Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement) {
		NEX_ASSERT (parentType == CommandDelegate::SHADER_BLOCK);
		Shader::StreamRequest* shader = (static_cast<Shader::StreamRequest*>(parentParam));
		String value;
		StringUtils::TokenIterator it = 0;
		it = StringUtils::NextWord(statement.paramContext, value, it);
		if (it != String::npos) {
			GpuProgram::Type type = GpuProgram::TYPE_VERTEX;
			if (statement.command == "FragmentProgram")
				type = GpuProgram::TYPE_FRAGMENT;
			else if (statement.command == "GeometryProgram")
				type = GpuProgram::TYPE_GEOMETRY;
			shader->SetProgramSource(type, URL(value));
		}
	}

} /* namespace ShaderCompiler */
