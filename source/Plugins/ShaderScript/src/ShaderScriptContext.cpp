/*
 * ShaderScriptContext.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include <ShaderScriptContext.h>

namespace ShaderScript {

ShaderScriptContext::ShaderScriptContext(ShaderTemplate::LoadStreamRequest* shader, const ShaderTemplate::ID& id, const ShaderScriptConfig& config) :
	_config(config),
	verifyID(true){
	this->shader = shader;
	String effectPath = shader->GetAssetLocator().GetComputedFilePath();
	String shaderPath = effectPath + "../Shaders/";
	String cbufferPath = effectPath + "../ConstBuffers/";
	StringUtils::PushBackWord(programIncludePath, shaderPath);
	StringUtils::PushBackWord(programIncludePath,
			FileSystem::ArchiveEngineData + "/Scripts/Shaders/");
	StringUtils::PushBackWord(cbufferIncludePath, cbufferPath);
	StringUtils::PushBackWord(cbufferIncludePath,
			FileSystem::ArchiveEngineData + "/Scripts/ConstBuffers/");
	for (uint32 i = 0; i < (uint32)Pass::ProgramStage::STAGE_COUNT; ++i)
		activeStages[i] = false;
	cbIsAutoParam = false;
	shaderId = id;
}

InputStreamPtr ShaderScriptContext::FetchConstBuffer(const String& name) {
	StringUtils::TokenIterator it = ConstMultiStringHelper::It(cbufferIncludePath);
	String store;
	InputStreamPtr retFile;
	while(it.HasNext(store)) {
		URL url(name, ".cscript", store);
		retFile = FileSystem::Instance().OpenRead(url);
		if (retFile)
			return retFile;
	}

	Warn("Could not open constant buffer file: " + name);
	return retFile;
}

InputStreamPtr ShaderScriptContext::FetchProgram(const String& name,
		RenderManager::ShaderLanguage lang,
		Pass::ProgramStage::Type stageType) {

	StringUtils::TokenIterator it = ConstMultiStringHelper::It(programIncludePath);
	String store;
	URL url;
	InputStreamPtr retFile;
	

	while(it.HasNext(store)) {

		switch (lang) {
		case RenderManager::SPP_GLSL:
			url = store + "GLSL/" + name + ".glsl";
			break;
		case RenderManager::SPP_HLSL:
			url = store + "HLSL/" + name + ".glsl";
			break;
		}

		retFile = FileSystem::Instance().OpenRead(url);
		if (retFile)
			return retFile;
	}

	Warn("Could not open program file: " + name + " @location: " + url.ToString());
	return retFile;
}

void ShaderScriptContext::SetRegionsAsSource(Pass::ProgramStage::Type type,
		const StringUtils::WordList& regionNames) {
	StringUtils::TokenIterator regIt = ConstMultiStringHelper::It(regionNames);
	String name;
	String value[(uint32)RenderManager::ShaderLanguage::SPP_COUNT];
	while (regIt.HasNext(name)) {
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

void ShaderScriptContext::AddRegion(const String& name,
		RenderManager::ShaderLanguage lang, String&& value) {
	regions.emplace(name,
			std::pair<RenderManager::ShaderLanguage, String>(std::move(lang), std::move(value)));
}

void ShaderScriptContext::RemoveRegion(const String& name) {
	regions.erase(name);
}

} /* namespace ShaderScript */
