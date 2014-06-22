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

namespace ShaderCompiler {

void ShaderScript::SetRegionAsSource(GpuProgram::Type type,
		const String& name) {
	auto it = regions.find(name);
	if (it == regions.end()) {
		Error("Requested region does not exist: " + name);
		return;
	}

	shader->SetProgramSource(type, std::move((*it).second));
}

void ShaderScript::EnterScript(ScriptParser::ScriptContext& block) {
	block.ParseRegions(this);
}

void ShaderScript::EnterRegion(ScriptParser::RegionContext& ctx) {
	const String& name = ctx.GetName();
	String::size_type pos;
	if (name == _SS(REGION_SHADER)) {
		ctx.ParseStatements(this);
	} else if ((pos = name.find(languageContext)) != String::npos) {
		auto pos = name.find(':');
		if (pos != String::npos) {
			String regionName = name.substr(pos + 1);
			String regionValue;
			ctx.ParseText(regionValue);
			regions.emplace(std::move(regionName), std::move(regionValue));
		}
	}
}

void ShaderScript::EnterStatement(ScriptParser::StatementContext& ctx) {
	if (ctx.GetCommand() == _SS(CMD_SHADER)) {
		String name;
		StringUtils::NextWord(ctx.GetParamList(), name);
		ShaderCompiler::ShaderListener sl(this, name);
		ctx.ParseBlock(&sl);
	}
}

} /* namespace nextar */
