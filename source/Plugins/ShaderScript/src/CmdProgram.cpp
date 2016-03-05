/*
 * CmdProgram.cpp
 *
 *  Created on: 06-Mar-2016
 *      Author: obhi
 */

#include <CmdProgram.h>
#include <ShaderScriptContext.h>

namespace ShaderScript {

bool ShaderScript::CmdOption::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	String name, condition;

	auto it = h.Iterate();

	if (it.HasNext(name)) {
		if (it.HasNext(condition)) {
			String desc = StringUtils::GetTaggedVal(_SS(TAG_DESC), it);
			c->shader->AddMacro(name, condition, desc);
		}
	}
	return true;
}

bool ShaderScript::CmdDefine::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	String name;

	auto it = h.Iterate();
	if (it.HasNext(name)) {
		// add it to predefined
		LanguageTranslator::Instance().AddMacro(c, name);
	}

	return true;
}

InputStreamPtr ShaderScript::SubProgramType::AddProgram(
		const String& programName, int i, const String& src,
		ShaderScriptContext* c) const {
	InputStreamPtr program = c->FetchProgram(programName,
			(RenderManager::ShaderLanguage) (i), stage);
	if (program) {
		const void* buffer = nullptr;
		size_t num = program->AcquireBuffer(buffer);
		String programString;
		programString.reserve(num + 1);
		programString.assign((const char*) (buffer), num);
		c->AddRegion(src, (RenderManager::ShaderLanguage) (i),
				std::move(programString));
		program->ReleaseBuffer(buffer);
	}
	return program;
}

bool ShaderScript::SubProgramType::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto& blendOp = c->blendState.blendOp[c->blendState.numRenderTargets++];
	String programName;
	auto it = h.Iterate();

	String src;
	switch (stage) {
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

	if (it.HasNext(programName)) {
		if (programName != _SS(ARG_NO_COMMON)) {
			it = h.Iterate();
			for(int i = 0; i < RenderManager::ShaderLanguage::SPP_COUNT; ++i) {
				AddProgram("Common", i, src, c);
			}
		}
	}
	while (	it.HasNext(programName) )  {
		for(int i = 0; i < RenderManager::ShaderLanguage::SPP_COUNT; ++i) {
			AddProgram(programName, i, src, c);
		}
	}

	c->SetStageActive(stage, true);

	return true;
}

} /* namespace ShaderScript */
