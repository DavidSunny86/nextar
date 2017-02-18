/*
 * CmdProgram.cpp
 *
 *  Created on: 06-Mar-2016
 *      Author: obhi
 */

#include <CmdProgram.h>
#include <ShaderScriptContext.h>
#include <LanguageTranslator.h>

namespace ShaderScript {

bool ShaderScript::CmdActivate::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {

	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String name;

	if (it.HasNext(name)) {
		// add it to predefined
		if (name == "always") {
			while (it.HasNext(name))
				LanguageTranslator::Instance().AddMacro(c, name);
		} else {

			String value;
			String defines;
			MultiStringHelper options(defines);
			while (it.HasNext(value))
				options.PushBack(value);
			c->shader->AddCompilerOption(name, defines);
		}
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
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	auto& blendOp = c->blendState.blendOp[c->blendState.numRenderTargets++];
	String programName;

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
			it = command->GetParameters().Iterate(c->templateResolver);
			for(int i = 0; i < RenderManager::ShaderLanguage::SPP_COUNT; ++i) {
				String commonProgName;
				auto programIt = ConstMultiStringHelper::It(c->GetProgramPrefix(stage));
				while(it.HasNext(commonProgName))
					AddProgram(commonProgName, i, src, c);
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

bool RegShaderRegion::BeginExecute(CommandContext* pContext,
	const ASTRegion* region, bool isText) const {
	// let the commands execute
	return true;
}

bool RegProgramRegion::BeginExecute(CommandContext* pContext, 
	const ASTRegion* region, bool isText) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	if (isText) {
		const ASTTextRegion* reg = static_cast<const ASTTextRegion*>(region);
		String src;
		const String& fullName = region->GetName();
		size_t p = fullName.find_first_of(':');
		if (p != String::npos) {
			p++;
			String subname = fullName.substr(0, p-1);
			size_t t = fullName.find_first_of(':', p);
			if (t != String::npos) {
				src = fullName.substr(p, t - p) + "-" + subname;
				p = t + 1;
			} else
				return false;
		}
		else
			return false;

		size_t n = fullName.find_first_of(':', p);
		
		RenderManager::ShaderLanguage l = GetLanguage(fullName.substr(p, n));
		if (l == RenderManager::ShaderLanguage::SSP_ALL) {
			for (uint32 i = 0; i < RenderManager::ShaderLanguage::SPP_COUNT; ++i) {
				String v = reg->GetContents();
				c->AddRegion(src, (RenderManager::ShaderLanguage) (i),	std::move(v));
			}
		} else if (l != RenderManager::ShaderLanguage::SPP_UNKNOWN) {
			String v = reg->GetContents();
			c->AddRegion(src, (RenderManager::ShaderLanguage) (l), std::move(v));
		}
	}
	// @todo Make this true if any other region has to be handled
	return false;
}

RenderManager::ShaderLanguage RegProgramRegion::GetLanguage(const String& v) {
	if (v == "all")
		return RenderManager::ShaderLanguage::SSP_ALL;
	if (v == "hlsl")
		return RenderManager::ShaderLanguage::SPP_HLSL;
	if (v == "glsl")
		return RenderManager::ShaderLanguage::SPP_GLSL;
	return RenderManager::ShaderLanguage::SPP_UNKNOWN;
}

} /* namespace ShaderScript */
