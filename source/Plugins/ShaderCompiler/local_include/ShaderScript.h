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

	typedef multimap<String, std::pair<RenderManager::ShaderProgramLanguage, String> >::type SourceRegionMap;
	ShaderTemplate::StreamRequest* shader;
	SourceRegionMap regions;
public:

	ShaderScript(ShaderTemplate::StreamRequest* s) :
			shader(s) {
	}

	void SetRegionAsSource(Pass::ProgramStage, const String& name);

	inline ShaderTemplate::StreamRequest* GetRequest() {
		return shader;
	}

	virtual void EnterRegion(ScriptParser::RegionContext& regionCtx);
	virtual void EnterScript(ScriptParser::ScriptContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& ctx);

	~ShaderScript() {
	}
};

} /* namespace nextar */

#endif /* SHADERSCRIPT_H_ */
