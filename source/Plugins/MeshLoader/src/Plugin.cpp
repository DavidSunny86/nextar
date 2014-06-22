/* 
 * File:   Plugin.cpp
 * Author: obhi
 * 
 * Created on May 8, 2011, 2:23 AM
 */

#include <EngineHeaders.h>
#include <Plugin.h>
#include <MeshLoaderImpl.h>

NEX_IMPLEMENT_PLUGIN(MeshLoader){
return &MeshLoader::Plugin::plugin;
}

namespace MeshLoader {

Plugin Plugin::plugin;

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
}

void Plugin::LicenseRenewed() {
	MeshAsset::Loader::RegisterFactory("MESH", &MeshLoaderImpl::impl);
}

bool Plugin::LicenseExpired() {
	MeshAsset::Loader::UnregisterFactory("MESH");
	return true;
}

void Plugin::Close() {
}

void Plugin::Dispose() {
}
}
