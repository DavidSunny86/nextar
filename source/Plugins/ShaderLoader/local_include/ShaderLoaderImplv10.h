/*
 * ShaderLoaderImplv10.h
 *
 *  Created on: 29-Jul-2014
 *      Author: obhi
 */

#ifndef SHADERLOADERIMPLV10_H_
#define SHADERLOADERIMPLV10_H_

#include <RenderEngineHeaders.h>
#include <Asset.h>

using namespace nextar;
namespace ShaderLoader {

/*
 *
 */
class ShaderLoaderImplv1_0: public nextar::AssetLoaderImpl {
	NEX_LOG_HELPER(ShaderLoaderImplv1_0);
public:
	ShaderLoaderImplv1_0();
	virtual ~ShaderLoaderImplv1_0();
		
	void ReadPass(ShaderAsset::StreamRequest* request, ChunkInputStream& ser);
	virtual void Load(InputStreamPtr&, AssetLoader& loader);

protected:
	RenderManager::ShaderLanguage language;
};

} /* namespace ShaderLoader */

#endif /* SHADERLOADERIMPLV10_H_ */
