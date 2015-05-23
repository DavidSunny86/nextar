/* 
 * File:   Plugin.cpp
 * Author: obhi
 * 
 * Created on May 8, 2011, 2:23 AM
 */
#include <FbxMeshImporter.h>
#include <Plugin.h>
#include <MeshTemplate.h>
#include <FbxMeshLoaderImplBase.h>

NEX_IMPLEMENT_PLUGIN(FbxMeshImporter, FbxMeshImporter::Plugin);

namespace FbxMeshImporter {

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
}

void Plugin::LicenseRenewed() {
	AssetLoader::RegisterFactory("FBX", MeshTemplate::Traits::CLASS_ID,
			&FbxMeshLoaderImplBase::loader);
}

bool Plugin::LicenseExpired() {
	AssetLoader::UnregisterFactory("FBX", MeshTemplate::Traits::CLASS_ID);
	return true;
}

void Plugin::Close() {
}

void Plugin::Dispose() {
}
}
