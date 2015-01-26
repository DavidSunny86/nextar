/*
 * ShaderScript.h
 *
 *  Created on: 19-May-2014
 *      Author: obhi
 */

#ifndef SHADERSCRIPT_H_
#define SHADERSCRIPT_H_

#include <CommonTypes.h>

namespace ShaderCompiler {

/**
 * @effect
 * shader <name> {
 * 	pass <pass> {
 * 		program {
 * 			vertex_program VS
 * 			fragment_program PS
 * 		}
 * 	}
 * }
 *
 * @script:glsl:VS
 * @script:glsl:PS
 */

class ShaderScript: public ScriptParser::RegionListener,
		public ScriptParser::ScriptListener,
		public ScriptParser::StatementListener {

	NEX_LOG_HELPER (ShaderScript);

	typedef multimap<String, std::pair<RenderManager::ShaderLanguage, String> >::type SourceRegionMap;
	StringUtils::WordList programIncludePath;
	StringUtils::WordList cbufferIncludePath;
	ShaderTemplate::LoadStreamRequest* shader;
	SourceRegionMap regions;
public:

	ShaderScript(ShaderTemplate::LoadStreamRequest* s);

	void SetRegionsAsSource(Pass::ProgramStage, const StringUtils::WordList& regionNames);

	inline ShaderTemplate::LoadStreamRequest* GetRequest() {
		return shader;
	}

	InputStreamPtr FetchConstBuffer(const String& name);
	InputStreamPtr FetchProgram(const String& name, RenderManager::ShaderLanguage lang,
			Pass::ProgramStage stage);

	void AddRegion(const String& name, RenderManager::ShaderLanguage lang, String&& value);
	void RemoveRegion(const String& name);

	virtual void EnterRegion(ScriptParser::RegionContext& regionCtx);
	virtual void EnterScript(ScriptParser::ScriptContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& ctx);

	~ShaderScript() {
	}
};

} /* namespace nextar */

#endif /* SHADERSCRIPT_H_ */
