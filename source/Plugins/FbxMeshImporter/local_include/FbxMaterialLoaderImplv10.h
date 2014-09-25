/*
 * FbxMaterialLoaderImplv10.h
 *
 *  Created on: 14-Sep-2014
 *      Author: obhi
 */

#ifndef FBXMATERIALLOADERIMPLV10_H_
#define FBXMATERIALLOADERIMPLV10_H_

#include <FbxMeshImporter.h>
#include <Asset.h>

class FbxMaterialLoaderImplv1_0: public nextar::AssetLoaderImpl {
public:
	FbxMaterialLoaderImplv1_0(FbxSurfaceMaterial* pFbxMat);
	virtual ~FbxMaterialLoaderImplv1_0();
};

#endif /* FBXMATERIALLOADERIMPLV10_H_ */
