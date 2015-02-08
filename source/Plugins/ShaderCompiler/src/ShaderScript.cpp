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

ShaderScript::ShaderScript(ShaderTemplate::LoadStreamRequest* s) :
		shader(s) {

	String effectPath = s->GetAssetLocator().GetRelativePath();
	String shaderPath = effectPath + "../Shaders/";
	String cbufferPath = effectPath + "../ConstBuffers/";
	StringUtils::PushBackWord(programIncludePath, shaderPath);
	StringUtils::PushBackWord(programIncludePath, FileSystem::ArchiveEngineData + "/Scripts/Shaders/");
	StringUtils::PushBackWord(cbufferIncludePath, cbufferPath);
	StringUtils::PushBackWord(cbufferIncludePath, FileSystem::ArchiveEngineData + "/Scripts/ConstBuffers/");

	for (uint32 i = 0; i < (uint32)Pass::ProgramStage::STAGE_COUNT; ++i)
		activeStages[i] = false;
}

void ShaderScript::SetRegionsAsSource(Pass::ProgramStage type,
		const StringUtils::WordList& regionNames) {
	StringUtils::TokenIterator regIt = 0;
	String name;
	String value[(uint32)RenderManager::ShaderLanguage::SPP_COUNT];
	while ((regIt = StringUtils::NextWord(regionNames, name, regIt)) != String::npos) {
		auto it = regions.equal_range(name);
		for(; it.first != it.second; ++it.first) {
			value[(*it.first).second.first] += (*it.first).second.second;
		}
	}

	for(uint32 i = 0; i < (uint32)RenderManager::ShaderLanguage::SPP_COUNT; ++i) {
		shader->SetProgramSource(type,
				(RenderManager::ShaderLanguage)i,
				std::move(value[i]));
	}
}

void ShaderScript::EnterScript(ScriptParser::ScriptContext& block) {
	block.ParseRegions(this);
}

void ShaderScript::EnterRegion(ScriptParser::RegionContext& ctx) {
	//const String& name = ctx.GetName();
	ctx.ParseStatements(this);
	/*} else {
		String script = _SS(PROGRAM_SCRIPT);
		if (!name.compare(0, script.length(), script)) {
			script = name.substr(script.length()+1);
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
		} else
			ctx.Discard();
	}*/
}

void ShaderScript::EnterStatement(ScriptParser::StatementContext& ctx) {
	if (ctx.GetCommand() == _SS(CMD_SHADER)) {
		String name;
		StringUtils::NextWord(ctx.GetParamList(), name);
		ShaderListener sl(this, name);
		ctx.ParseBlock(&sl);
	}
}

InputStreamPtr ShaderScript::FetchConstBuffer(const String& name) {
	StringUtils::TokenIterator it = 0;
	String store;
	URL url;
	InputStreamPtr retFile;
	while((it = StringUtils::NextWord(cbufferIncludePath, store, it)) != String::npos) {
		url = store + name + ".const";
		retFile = FileSystem::Instance().OpenRead(url);
		if (retFile)
			return retFile;
	}

	Warn("Could not open constant buffer file: " + name + " @location: " + url.ToString());
	return retFile;
}

InputStreamPtr ShaderScript::FetchProgram(const String& name,
		RenderManager::ShaderLanguage lang,
		Pass::ProgramStage stageType) {

	StringUtils::TokenIterator it = 0;
	String store;
	URL url;
	InputStreamPtr retFile;
	const char* stage = 0;
	switch(stageType) {
	case Pass::ProgramStage::STAGE_VERTEX:
		stage = ".vert"; break;
	case Pass::ProgramStage::STAGE_FRAGMENT:
		stage = ".frag"; break;
	case Pass::ProgramStage::STAGE_GEOMETRY:
		stage = ".geom"; break;
	case Pass::ProgramStage::STAGE_HULL:
		stage = ".hull"; break;
	case Pass::ProgramStage::STAGE_DOMAIN:
		stage = ".dom"; break;
	default:
		Warn("Invalid stage: " + name);
		return retFile;
	}

	while((it = StringUtils::NextWord(programIncludePath, store, it)) != String::npos) {
		
		switch (lang) {
		case RenderManager::SPP_GLSL:
			url = store + "GLSL/" + name + stage;
			break;
		case RenderManager::SPP_HLSL:
			url = store + "HLSL/" + name + stage;
			break;
		}

		retFile = FileSystem::Instance().OpenRead(url);
		if (retFile)
			return retFile;
	}

	Warn("Could not open program file: " + name + " @location: " + url.ToString());
	return retFile;

}

void ShaderScript::AddRegion(const String& name,
		RenderManager::ShaderLanguage lang, String&& value) {
	regions.emplace(name,
			std::pair<RenderManager::ShaderLanguage, String>(std::move(lang), std::move(value)));
}

void ShaderScript::RemoveRegion(const String& name) {
	regions.erase(name);
}

} /* namespace nextar */
