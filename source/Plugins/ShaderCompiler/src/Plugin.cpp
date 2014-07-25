/*
 * Plugin.cpp
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <Plugin.h>

NEX_IMPLEMENT_PLUGIN(ShaderCompiler, ShaderCompiler::Plugin);

namespace ShaderCompiler {

ShaderLoaderImplv1_0 Plugin::shaderLoaderImpl;

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
	AssetLoader::RegisterFactory("NFX", ShaderAsset::Traits::CLASS_ID,
			&shaderLoaderImpl);
}

bool Plugin::LicenseExpired() {
	AssetLoader::UnregisterFactory("NFX", ShaderAsset::Traits::CLASS_ID);
	return true;
}

void Plugin::Dispose() {
}
} /* namespace ShaderCompiler */
