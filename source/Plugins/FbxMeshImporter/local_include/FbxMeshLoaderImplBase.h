/*
 * FbxMeshLoaderImplBase.h
 *
 *  Created on: 31-Aug-2014
 *      Author: obhi
 */

#ifndef FBXMESHLOADERIMPLBASE_H_
#define FBXMESHLOADERIMPLBASE_H_

#include <FbxMeshImporter.h>
#include <Asset.h>

namespace FbxMeshImporter {

/*
 *
 */
class FbxMeshLoaderImplBase: public nextar::AssetLoaderImpl {
	NEX_LOG_HELPER(FbxMeshLoaderImplBase);
public:
	FbxMeshLoaderImplBase();
	virtual ~FbxMeshLoaderImplBase();

	virtual void Load(InputStreamPtr&, AssetLoader& loader);

	static FbxMeshLoaderImplBase loader;

protected:


	FbxManager* mManager;
	FbxIOSettings* mIOSettings;

};

} /* namespace FbxMeshImporter */

#endif /* FBXMESHLOADERIMPLBASE_H_ */
