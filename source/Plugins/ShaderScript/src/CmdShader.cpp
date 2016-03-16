/*
 * CmdShader.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include <CmdShader.h>
#include <ShaderScriptContext.h>
#include <LanguageTranslator.h>

namespace ShaderScript {


bool ShaderScript::CmdShader::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	String name = h.Get(0);
	SharedComponent::ID id = SharedComponent::ToID(name);
	if (id.name == c->shaderId.name &&
		id.group == c->shaderId.group) {
		return true;
	}
	return false;
}

void ShaderScript::CmdShader::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
}

bool ShaderScript::CmdTags::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = h.Iterate();
	uint32 flags = 0;
	String value;
	while (it.HasNext(value)) {
		StringUtils::ToLower(value);
		flags |= Helper::GetTag(value);
	}

	if (flags) {
		c->shader->SetRenderFlags(flags);
	}
	return true;
}

bool ShaderScript::CmdPass::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	c->shader->AddPass(NamedObject::AsyncStringID(h.Get(0)));
	return true;
}

void CmdPass::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	String predefs = _SS(REG_PREDEFS);
	String defines = _SS(REG_DEFINE);
	String cbuffer = _SS(REG_CBUFFER);;
	ShaderScriptContext* shaderScript = static_cast<ShaderScriptContext*>(pContext);
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
			LanguageTranslator::Instance().AddPredefs(shaderScript, (Pass::ProgramStage)i);
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

bool CmdImportConstBuffer::BeginExecute(CommandContext* pContext,
	const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = h.Iterate();
	String value;
	while (it.HasNext(value)) {
		InputStreamPtr is = c->FetchConstBuffer(value);
		if (is) {
			LanguageTranslator::Instance().TranslateConstantBuffer(c, value, is);
		}
	}
	return true;
}

bool CmdConstBuffer::BeginExecute(CommandContext* pContext,
	const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	return LanguageTranslator::ConstBuffer_BeginExecute(c, command);
}

void CmdConstBuffer::EndExecute(CommandContext* pContext,
	const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	LanguageTranslator::ConstBuffer_EndExecute(c, command);
}

bool CmdDeclare::BeginExecute(CommandContext* pContext,
	const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	return LanguageTranslator::Declare_BeginExecute(c, command);
}

} /* namespace ShaderScript */
