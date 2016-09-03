/*
 * Plugin.cpp
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <Plugin.h>

NEX_IMPLEMENT_PLUGIN(ResourcePacker, ResourcePacker::Plugin);

namespace ResourcePacker {

ShaderLoaderImplv1_0 Plugin::shaderLoaderImpl;

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
	Trace("Loaded shader loader 1.0.");
}

void Plugin::Close() {
	Trace("Unloading shader loader 1.0.");
}

void Plugin::LicenseRenewed() {
	AssetLoader::RegisterFactory("nexfx", ShaderAsset::Traits::CLASS_ID,
			&shaderLoaderImpl);
}

bool Plugin::LicenseExpired() {
	AssetLoader::UnregisterFactory("nexfx", ShaderAsset::Traits::CLASS_ID);
	return true;
}

void Plugin::Dispose() {
}

} /* namespace ResourcePacker */
