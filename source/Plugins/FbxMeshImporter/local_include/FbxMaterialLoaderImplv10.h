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

	FbxMaterialLoaderImplv1_0(FbxSurfaceMaterial* pFbxMat, const URL& kMeshPath);
	virtual ~FbxMaterialLoaderImplv1_0();

	virtual void Load(InputStreamPtr&, AssetLoader&);

	void CreateFromMaterial(FbxSurfaceMaterial* pFbxMat);
	bool ParseMap(FbxSurfaceMaterial* pFbxMat, const char* mapName, TextureAsset::ID& id, URL& location);

protected:
	Color diffuseColor;
	float gloss;

	bool hasDiffuseMap;
	URL diffuseLocation;
	TextureAsset::ID diffuseMapId;

	bool hasSpecularMap;
	URL specularLocation;
	TextureAsset::ID specularMapId;

	bool hasNormalMap;
	URL normalLocation;
	TextureAsset::ID normalMapId;

	URL shaderLocation;
	ShaderAsset::ID shaderId;
	NameValueMap params;

	String relativePathOfMesh;
};

}

#endif /* FBXMATERIALLOADERIMPLV10_H_ */
