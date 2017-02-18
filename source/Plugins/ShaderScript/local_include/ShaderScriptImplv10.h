/*
 * ShaderScriptImplv10.h
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_SHADERSCRIPTIMPLV10_H_
#define PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_SHADERSCRIPTIMPLV10_H_

#include <ShaderScript.h>
#include <Asset.h>

namespace ShaderScript {

class ShaderScriptImplv1_0: public nextar::AssetLoaderImpl {
public:
	ShaderScriptImplv1_0();
	virtual ~ShaderScriptImplv1_0();

	virtual void Configure(const Config&);
	virtual void Load(InputStreamPtr& input, AssetLoader& shader);
	void RegisterDictionary();
	void UnregisterDictionary();

protected:
	ShaderScriptConfig _config;
	RegionHandler _rootShaderScript;
	BlockCommandHandler _rootTranslatorCommandHandler;
	RegionHandler _rootTranslator;
};

} /* namespace ShaderScript */

#endif /* PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_SHADERSCRIPTIMPLV10_H_ */
