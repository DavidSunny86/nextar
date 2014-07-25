/*
 * MaterialLoaderImplv10.h
 *
 *  Created on: 18-Jul-2014
 *      Author: obhi
 */

#ifndef MATERIALLOADERIMPLV10_H_
#define MATERIALLOADERIMPLV10_H_

#include <FactoryPlugin.h>
#include <ScriptParser.h>
#include <Asset.h>

namespace MaterialLoader {

/*
 *
 */
class MaterialLoaderImplv1_0: public nextar::AssetLoaderImpl {
public:
	MaterialLoaderImplv1_0();
	virtual ~MaterialLoaderImplv1_0();

	virtual void Configure(const nextar::Config&);
	virtual void Load(nextar::InputStreamPtr& input, nextar::AssetLoader& shader);
};

} /* namespace MaterialLoader */

#endif /* MATERIALLOADERIMPLV10_H_ */
