/*
 * FbxMaterialLoaderImplv10.cpp
 *
 *  Created on: 14-Sep-2014
 *      Author: obhi
 */

#include <FbxMeshImporter.h>
#include <FbxMaterialLoaderImplv10.h>

namespace FbxMeshImporter {

FbxMaterialLoaderImplv1_0::FbxMaterialLoaderImplv1_0(FbxSurfaceMaterial* pFbxMat) {

	CreateFromMaterial(pFbxMat);
}

FbxMaterialLoaderImplv1_0::~FbxMaterialLoaderImplv1_0() {
	// TODO Auto-generated destructor stub
}

void FbxMaterialLoaderImplv1_0::Load(InputStreamPtr& input,
		AssetLoader& assetLoader) {

	delete this;
}

bool FbxMaterialLoaderImplv1_0::ParseMap(FbxSurfaceMaterial* pFbxMat,
		const char* mapName, TextureAsset::ID& id, URL& location) {

	FbxProperty lProperty = pFbxMat->FindProperty(mapName);

	int lLayeredTextureCount = lProperty.GetSrcObjectCount<FbxLayeredTexture>();
	if(lLayeredTextureCount > 0) {
		for(int j=0; j<lLayeredTextureCount; ++j) {
			FbxLayeredTexture *lLayeredTexture = lProperty.GetSrcObject<FbxLayeredTexture>(j);
			int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
			for(int k =0; k<lNbTextures; ++k) {
				String kName = lLayeredTexture->GetNameOnly().Buffer();
				String kPath = lLayeredTexture->GetName();
				id.name = NamedObject::AsyncStringID(kName);
				location = URL("Textures", kPath);
				return true;
	        }
	    }
	}
	else {
		//no layered texture simply get on the property
		int lNbTextures = lProperty.GetSrcObjectCount<FbxTexture>();
		if(lNbTextures > 0) {
			for(int j =0; j<lNbTextures; ++j) {
				FbxTexture* lTexture = lProperty.GetSrcObject<FbxTexture>(j);
				if(lTexture) {
					String kName = lTexture->GetNameOnly().Buffer();
					String kPath = lTexture->GetName();
					id.name = NamedObject::AsyncStringID(kName);
					location = URL("Textures", kPath);
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

	ParseMap(pFbxMat, FbxSurfaceMaterial::sDiffuse, diffuseMapId, diffuseLocation);
}

}
