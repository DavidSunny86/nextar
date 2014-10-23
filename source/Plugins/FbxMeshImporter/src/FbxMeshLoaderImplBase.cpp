/*
 * FbxMeshLoaderImplBase.cpp
 *
 *  Created on: 31-Aug-2014
 *      Author: obhi
 */
#include <FbxMeshImporter.h>
#include <FbxMeshLoaderImplBase.h>
#include <FbxMeshLoaderImplv10.h>

namespace FbxMeshImporter {

FbxMeshLoaderImplBase FbxMeshLoaderImplBase::loader;

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

	FbxMeshLoaderImplv1_0 mainLoader(mManager, stream,
			loader.GetRequestPtr()->GetParameterValue("password"));
	if(!mainLoader.LoadScene()) {
		Warn("Failed to load!");
		NEX_THROW_GracefulError(EXCEPT_FAILED_TO_CREATE_OBJECT);
	}

	mainLoader.ParseContent();
	loader.GetRequestPtr()->SetCompleted(true);

}

} /* namespace FbxMeshImporter */
