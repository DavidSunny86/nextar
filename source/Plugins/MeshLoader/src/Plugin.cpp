/* 
 * File:   Plugin.cpp
 * Author: obhi
 * 
 * Created on May 8, 2011, 2:23 AM
 */

#include <EngineHeaders.h>
#include <Plugin.h>
#include <MeshLoaderImpl.h>

NEX_IMPLEMENT_PLUGIN(MeshLoader, MeshLoader::Plugin);

namespace MeshLoader {

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
}

void Plugin::LicenseRenewed() {
	AssetLoader::RegisterFactory("nexmesh", MeshAsset::CLASS_ID, &MeshLoaderImpl::impl);
}

bool Plugin::LicenseExpired() {
	AssetLoader::UnregisterFactory("nexmesh", MeshAsset::CLASS_ID);
	return true;
}

void Plugin::Close() {
}

void Plugin::Dispose() {
}
}
