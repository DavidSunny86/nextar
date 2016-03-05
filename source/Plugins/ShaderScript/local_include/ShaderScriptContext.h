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


class ShaderScriptContext : public CommandContext {
	NEX_LOG_HELPER(ShaderScriptContext);

public:

	ShaderScriptContext(ShaderTemplate::LoadStreamRequest* shader, const ShaderTemplate::ID&);

	typedef multimap<String, std::pair<RenderManager::ShaderLanguage, String> >::type SourceRegionMap;

	StringUtils::WordList programIncludePath;
	StringUtils::WordList cbufferIncludePath;

	inline bool IsStageActive(Pass::ProgramStage stage) const {
		return activeStages[(uint32)stage];
	}

	inline void SetStageActive(Pass::ProgramStage stage, bool active) {
		activeStages[(uint32)stage] = active;
	}

	InputStreamPtr FetchConstBuffer(const String& name);
	InputStreamPtr FetchProgram(const String& name,
			RenderManager::ShaderLanguage lang,
			Pass::ProgramStage stageType);

	void SetRegionsAsSource(Pass::ProgramStage, const StringUtils::WordList& regionNames);
	void AddRegion(const String& name, RenderManager::ShaderLanguage lang, String&& value);
	void RemoveRegion(const String& name);

	String transientBuffer;
	bool cbIsAutoParam;
	SourceRegionMap regions;
	BlendState blendState;
	DepthStencilState depthStencilState;
	RasterState rasterState;
	TextureUnitParams textureState;
	String samplerName;

	ShaderTemplate::LoadStreamRequest* shader;
	ShaderTemplate::ID shaderId;

	bool activeStages[Pass::ProgramStage::STAGE_COUNT];
};

} /* namespace ShaderScript */

#endif /* PLUGINS_SHADERSCRIPT_SHADERSCRIPTCONTEXT_H_ */
