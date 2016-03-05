/*
 * MaterialScriptImplv10.h
 *
 *  Created on: 03-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_MATERIALSCRIPTIMPLV10_H_
#define PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_MATERIALSCRIPTIMPLV10_H_

#include <MaterialScript.h>
#include <Asset.h>

namespace MaterialScript {

class MaterialScriptImplv1_0: public nextar::AssetLoaderImpl {
public:
	MaterialScriptImplv1_0();
	virtual ~MaterialScriptImplv1_0();

	void RegisterDictionary();
	void UnregisterDictionary();

	virtual void Configure(const nextar::Config&);
	virtual void Load(nextar::InputStreamPtr& input, nextar::AssetLoader& material);

protected:
	RootBlockCommandHandler _root;
};

} /* namespace MaterialScript */

#endif /* PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_MATERIALSCRIPTIMPLV10_H_ */
