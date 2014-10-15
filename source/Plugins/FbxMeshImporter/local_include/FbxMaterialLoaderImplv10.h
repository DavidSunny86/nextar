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
namespace FbxMeshImporter {


class FbxMaterialLoaderImplv1_0: public nextar::AssetLoaderImpl,
	public AllocGeneral {
public:

	FbxMaterialLoaderImplv1_0(FbxSurfaceMaterial* pFbxMat);
	virtual ~FbxMaterialLoaderImplv1_0();

	virtual void Load(InputStreamPtr&, AssetLoader&);

	void CreateFromMaterial(FbxSurfaceMaterial* pFbxMat);
	bool ParseMap(FbxSurfaceMaterial* pFbxMat, const char* mapName, TextureAsset::ID& id, URL& location);

protected:
	Color diffuseColor;
	float gloss;

	URL diffuseLocation;
	TextureAsset::ID diffuseMapId;

	URL shaderLocation;
	ShaderAsset::ID shaderId;
	NameValueMap params;

};

}

#endif /* FBXMATERIALLOADERIMPLV10_H_ */
