/*
 * MaterialScript.h
 *
 *  Created on: 26-Jul-2014
 *      Author: obhi
 */

#ifndef MATERIALSCRIPT_H_
#define MATERIALSCRIPT_H_

namespace MaterialCompiler {

/*
 *
 */
class MaterialScript: public ScriptParser::RegionListener,
public ScriptParser::ScriptListener,
public ScriptParser::StatementListener {
	NEX_LOG_HELPER (MaterialScript);

	StringID materialName;
	MaterialTemplate::StreamRequest* material;
public:
	MaterialScript(MaterialTemplate::StreamRequest* request);

	inline MaterialTemplate::StreamRequest* GetRequest() {
		return material;
	}

	virtual void EnterRegion(ScriptParser::RegionContext& regionCtx);
	virtual void EnterScript(ScriptParser::ScriptContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& ctx);

};


} /* namespace MaterialCompiler */

#endif /* MATERIALSCRIPT_H_ */
