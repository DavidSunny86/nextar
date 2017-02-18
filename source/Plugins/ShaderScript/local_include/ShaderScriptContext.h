/*
 * ShaderScriptContext.h
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERSCRIPT_SHADERSCRIPTCONTEXT_H_
#define PLUGINS_SHADERSCRIPT_SHADERSCRIPTCONTEXT_H_

#include <ScriptStrings.h>
#include <ShaderScript.h>
#include <Helper.h>

namespace ShaderScript {

enum LanguageTranslatorType {
	GLSL_TRANSLATOR = 0,
	HLSL_TRANSLATOR,
	TRANSLATOR_COUNT,
};

class ShaderScriptContext : public CommandContext {
	NEX_LOG_HELPER(ShaderScriptContext);

public:
		

	ShaderScriptContext(ShaderTemplate::LoadStreamRequest* shader, const ShaderTemplate::ID&, const ShaderScriptConfig& params);

	typedef multimap<String, std::pair<RenderManager::ShaderLanguage, String> >::type SourceRegionMap;

	StringUtils::WordList programIncludePath;
	StringUtils::WordList cbufferIncludePath;

	inline bool IsStageActive(Pass::ProgramStage stage) const {
		return activeStages[(uint32)stage];
	}

	inline void SetStageActive(Pass::ProgramStage stage, bool active) {
		activeStages[(uint32)stage] = active;
	}

	inline const StringUtils::WordList& GetProgramPrefix(Pass::ProgramStage stage) {
		return _config.programPrefix[stage];
	}

	InputStreamPtr FetchConstBuffer(const String& name);
	InputStreamPtr FetchProgram(const String& name,
			RenderManager::ShaderLanguage lang,
			Pass::ProgramStage stageType);

	void SetRegionsAsSource(Pass::ProgramStage, const StringUtils::WordList& regionNames);
	void AddRegion(const String& name, RenderManager::ShaderLanguage lang, String&& value);
	void RemoveRegion(const String& name);
		
	bool verifyID;
	bool cbIsAutoParam;

	ASTParameter::Resolver templateResolver;
	SourceRegionMap regions;
	BlendState blendState;
	DepthStencilState depthStencilState;
	RasterState rasterState;
	TextureUnitParams textureState;
	String samplerName;
	String _transientBuffer[TRANSLATOR_COUNT];

	const ShaderScriptConfig& _config;
	ShaderTemplate::LoadStreamRequest* shader;
	ShaderTemplate::ID shaderId;

	bool activeStages[Pass::ProgramStage::STAGE_COUNT];
};

} /* namespace ShaderScript */

#endif /* PLUGINS_SHADERSCRIPT_SHADERSCRIPTCONTEXT_H_ */
