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
	Trace("Loaded material loader 1.0.");
}

void Plugin::Close() {
	Trace("Unloading material loader 1.0.");
}

void Plugin::LicenseRenewed() {
	AssetLoader::RegisterFactory("NEXMTL", MaterialAsset::Traits::CLASS_ID,
			&materialLoaderImpl);
}

bool Plugin::LicenseExpired() {
	AssetLoader::UnregisterFactory("NEXMTL", ShaderAsset::Traits::CLASS_ID);
	return true;
}

void Plugin::Dispose() {
}
} /* namespace ShaderCompiler */
