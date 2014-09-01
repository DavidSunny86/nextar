/*
 * FbxMeshLoaderImplBase.cpp
 *
 *  Created on: 31-Aug-2014
 *      Author: obhi
 */

#include <FbxMeshLoaderImplBase.h>

namespace FbxMeshImporter {

FbxMeshLoaderImplBase::FbxMeshLoaderImplBase() {
	mManager = FbxManager::Create();
	mIOSettings = FbxIOSettings::Create(mManager, IOSROOT);
	mManager->SetIOSettings(mIOSettings);
}

FbxMeshLoaderImplBase::~FbxMeshLoaderImplBase() {
	if (mManager)
		mManager->Destroy();
	mManager = nullptr;
	mIOSettings = nullptr;
}

void FbxMeshLoaderImplBase::Load(InputStreamPtr& stream,
		AssetLoader& loader) {

}

} /* namespace FbxMeshImporter */
