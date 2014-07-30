/*
 * MaterialLoaderImplv10.h
 *
 *  Created on: 29-Jul-2014
 *      Author: obhi
 */

#ifndef MATERIALLOADERIMPLV10_H_
#define MATERIALLOADERIMPLV10_H_

#include <EngineHeaders.h>
#include <Asset.h>

using namespace nextar;
namespace MaterialLoader {

/*
 *
 */
class MaterialLoaderImplv1_0: public nextar::AssetLoaderImpl {
	NEX_LOG_HELPER(MaterialLoaderImplv1_0);

public:
	MaterialLoaderImplv1_0();
	virtual ~MaterialLoaderImplv1_0();

	virtual void Load(InputStreamPtr&, AssetLoader& loader);
};

} /* namespace MaterialLoader */

#endif /* MATERIALLOADERIMPLV10_H_ */
