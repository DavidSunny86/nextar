/*
 * ShaderScript.cpp
 *
 *  Created on: 19-May-2014
 *      Author: obhi
 */

#include <BaseHeaders.h>
#include <CommonTypes.h>
#include <ShaderScript.h>
#include <ScriptStrings.h>
#include <ShaderListener.h>

namespace ShaderCompiler {

void ShaderScript::SetRegionAsSource(Pass::ProgramStage type,
		const String& name) {
	auto it = regions.equal_range(name);
	if (it.first == regions.end()) {
		Error("Requested region does not exist: " + name);
		return;
	}

	for(; it.first != it.second; ++it.first) {
		shader->SetProgramSource(type,
				(*it.first).second.first,
				std::move((*it.first).second.second) );
	}
}

void ShaderScript::EnterScript(ScriptParser::ScriptContext& block) {
	block.ParseRegions(this);
}

void ShaderScript::EnterRegion(ScriptParser::RegionContext& ctx) {
	const String& name = ctx.GetName();
	String::size_type pos;
	if (name == _SS(REGION_SHADER)) {
		ctx.ParseStatements(this);
	} else {
		String script = _SS(PROGRAM_SCRIPT);
		if (!name.compare(0, script.length(), script)) {
			script = name.substr(script.length());
			auto langRegName = StringUtils::Split(script, ':');
			RenderManager::ShaderLanguage lang = RenderManager::SPP_UNKNOWN;
			if (langRegName.first.compare(_SS(LANG_GLSL) ) == 0) {
				lang = RenderManager::SPP_GLSL;
			}
			else if (langRegName.first.compare(_SS(LANG_HLSL) ) == 0) {
				lang = RenderManager::SPP_HLSL;
			}
			if (lang != RenderManager::SPP_UNKNOWN) {
				String regionValue;
				ctx.ParseText(regionValue);
				// todo check if move semantics are being used
				regions.emplace(std::move(langRegName.second),
						std::pair<RenderManager::ShaderLanguage, String>(std::move(lang), std::move(regionValue)));
			}
		}
	}
}

void ShaderScript::EnterStatement(ScriptParser::StatementContext& ctx) {
	if (ctx.GetCommand() == _SS(CMD_SHADER)) {
		String name;
		StringUtils::NextWord(ctx.GetParamList(), name);
		ShaderListener sl(this, name);
		ctx.ParseBlock(&sl);
	}
}

} /* namespace nextar */
