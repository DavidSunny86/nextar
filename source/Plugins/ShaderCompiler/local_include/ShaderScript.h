/*
 * ShaderScript.h
 *
 *  Created on: 19-May-2014
 *      Author: obhi
 */

#ifndef SHADERSCRIPT_H_
#define SHADERSCRIPT_H_

namespace ShaderCompiler {

class ShaderScript: public ScriptParser::RegionListener,
		public ScriptParser::ScriptListener,
		public ScriptParser::StatementListener {

	NEX_LOG_HELPER (ShaderScript);

	StringRef languageContext;
	ShaderAsset::StreamRequest* shader;
	NameValueMap regions;
public:

	ShaderScript(const StringRef _languageContext,
			ShaderAsset::StreamRequest* s) :
			languageContext(_languageContext), shader(s) {
	}

	void SetRegionAsSource(GpuProgram::Type, const String& name);
	inline ShaderAsset::StreamRequest* GetRequest() {
		return shader;
	}

	virtual void EnterRegion(ScriptParser::RegionContext& regionCtx);
	virtual void EnterScript(ScriptParser::ScriptContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& ctx);
protected:
	~ShaderScript() {
	}
};

} /* namespace nextar */

#endif /* SHADERSCRIPT_H_ */
