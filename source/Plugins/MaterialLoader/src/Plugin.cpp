/*
 * Plugin.cpp
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */

#include <BaseHeaders.h>
#include <Plugin.h>

NEX_IMPLEMENT_PLUGIN(MaterialLoader, MaterialLoader::Plugin);

namespace MaterialLoader {

MaterialLoaderImplv1_0 Plugin::materialLoaderImpl;

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
	Trace("Loaded shader compiler 1.0.");
}

void Plugin::Close() {
	Trace("Unloading shader compiler 1.0.");
}

void Plugin::LicenseRenewed() {
	AssetLoader::RegisterFactory("MTL", MaterialAsset::Traits::CLASS_ID,
			&materialLoaderImpl);
}

bool Plugin::LicenseExpired() {
	AssetLoader::UnregisterFactory("MTL", ShaderAsset::Traits::CLASS_ID);
	return true;
}

void Plugin::Dispose() {
}
} /* namespace ShaderCompiler */
