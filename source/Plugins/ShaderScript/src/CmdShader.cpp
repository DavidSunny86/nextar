/*
 * CmdShader.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include <CmdShader.h>
#include <ShaderScriptContext.h>

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
		/*
		if (!tagsMap.size()) {
			tagsMap["background"] = RenderQueueFlags::BACKGROUND;
			tagsMap["deferred"] = RenderQueueFlags::DEFERRED;
			tagsMap["debug"] = RenderQueueFlags::DEBUG;
			tagsMap["deferred-lighting"] = RenderQueueFlags::DEFERRED_LIGHTING;
			tagsMap["forward"] = RenderQueueFlags::FORWARD;
			tagsMap["overlay"] = RenderQueueFlags::OVERLAY;
			tagsMap["compositor"] = RenderQueueFlags::COMPOSITOR;
			tagsMap["translucency"] = RenderQueueFlags::TRANSLUCENCY;
		}

		auto it = tagsMap.find(value);
		if (tagsMap.end() != it)
			flags |= (*it).second;*/
	}

	if (flags) {
		ShaderTemplate* shader = static_cast<ShaderTemplate*>(
				c->shader->GetStreamedObject());
		shader->SetRenderFlags(flags);
	}
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

	for (uint32 i = 0; i < Pass::STAGE_COUNT; ++i) {
		StringUtils::WordList words;
		String src;

		switch ((Pass::ProgramStage)i) {
		case Pass::ProgramStage::STAGE_DOMAIN:
			src = "domain"; break;
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

} /* namespace ShaderScript */
