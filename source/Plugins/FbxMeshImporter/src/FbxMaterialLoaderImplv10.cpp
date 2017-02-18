/*
 * FbxMaterialLoaderImplv10.cpp
 *
 *  Created on: 14-Sep-2014
 *      Author: obhi
 */

#include <FbxMeshImporter.h>
#include <FbxMaterialLoaderImplv10.h>

namespace FbxMeshImporter {

FbxMaterialLoaderImplv1_0::FbxMaterialLoaderImplv1_0(FbxSurfaceMaterial* pFbxMat, const URL& relativePath) :
hasDiffuseMap(false)
,hasNormalMap(false)
,hasSpecularMap(false) {
	relativePathOfMesh = relativePath.GetComputedFilePath();
	CreateFromMaterial(pFbxMat);
}

FbxMaterialLoaderImplv1_0::~FbxMaterialLoaderImplv1_0() {
	// TODO Auto-generated destructor stub
}

void FbxMaterialLoaderImplv1_0::PreLoadFailed(AssetLoader& assetLoader) {
	NEX_DELETE(this);
}

void FbxMaterialLoaderImplv1_0::Load(InputStreamPtr& input,
		AssetLoader& assetLoader) {
	assetLoader.GetRequestPtr()->SetCompleted(true);
	NEX_DELETE(this);
}

bool FbxMaterialLoaderImplv1_0::ParseMap(FbxSurfaceMaterial* pFbxMat,
		const char* mapName, TextureAsset::ID& id, URL& location) {

	FbxProperty lProperty = pFbxMat->FindProperty(mapName);

	int lLayeredTextureCount = lProperty.GetSrcObjectCount<FbxLayeredTexture>();
	if(lLayeredTextureCount > 0) {
		for(int j=0; j<lLayeredTextureCount; ++j) {
			FbxLayeredTexture *lLayeredTexture = lProperty.GetSrcObject<FbxLayeredTexture>(j);
			int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxFileTexture>();
			for(int k =0; k<lNbTextures; ++k) {
				FbxFileTexture* pIndiTexture = lLayeredTexture->GetSrcObject<FbxFileTexture>(k);
				String kName = pIndiTexture->GetNameOnly().Buffer();
				const char* pRelativePath = pIndiTexture->GetRelativeFileName();
				String kPath;
				if (!pRelativePath) {
					// find relative path here
					NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
				} else
					kPath = pRelativePath;
				id.name = StringUtils::GetStringID(kName);
				location = URL(relativePathOfMesh + kPath);
				return true;
	        }
	    }
	}
	else {
		//no layered texture simply get on the property
		int lNbTextures = lProperty.GetSrcObjectCount<FbxTexture>();
		if(lNbTextures > 0) {
			for(int j =0; j<lNbTextures; ++j) {
				FbxFileTexture* lTexture = lProperty.GetSrcObject<FbxFileTexture>(j);
				if(lTexture) {
					String kName = lTexture->GetNameOnly().Buffer();
					const char* pRelativePath = lTexture->GetRelativeFileName();
					String kPath;
					if (!pRelativePath) {
						// find relative path here
						NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
					} else
						kPath = pRelativePath;
					id.name = StringUtils::GetStringID(kName);
					location = URL(relativePathOfMesh + kPath);
					return true;
	            }
	        }
	    }
	}

	return false;
}

void FbxMaterialLoaderImplv1_0::CreateFromMaterial(FbxSurfaceMaterial* pFbxMat) {
	// we always go for phong right now
	// we just extract the diffuseAndGloss and diffuseMap
	if (pFbxMat->GetClassId().Is(FbxSurfacePhong::ClassId)) {
		FbxSurfacePhong* pPhong = static_cast<FbxSurfacePhong*>(pFbxMat);
		auto color = pPhong->Diffuse;
		diffuseColor.red = static_cast<float>(color.Get()[0]);
		diffuseColor.green = static_cast<float>(color.Get()[1]);
		diffuseColor.blue = static_cast<float>(color.Get()[2]);
		gloss = static_cast<float>(pPhong->Shininess.Get());
	} else if (pFbxMat->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
		FbxSurfaceLambert* pPhong = static_cast<FbxSurfaceLambert*>(pFbxMat);
		auto color = pPhong->Diffuse;
		diffuseColor.red = static_cast<float>(color.Get()[0]);
		diffuseColor.green = static_cast<float>(color.Get()[1]);
		diffuseColor.blue = static_cast<float>(color.Get()[2]);
		gloss = static_cast<float>(1.0);
	}

	hasDiffuseMap = ParseMap(pFbxMat, FbxSurfaceMaterial::sDiffuse, diffuseMapId, diffuseLocation);
	hasNormalMap = ParseMap(pFbxMat, FbxSurfaceMaterial::sNormalMap, normalMapId, normalLocation);
	hasSpecularMap = ParseMap(pFbxMat, FbxSurfaceMaterial::sSpecular, specularMapId, specularLocation);
}

}
