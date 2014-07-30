/*
 * Plugin.cpp
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */
#include <CommonTypes.h>
#include <BaseHeaders.h>
#include <ShaderTemplate.h>
#include <Plugin.h>

NEX_IMPLEMENT_PLUGIN(ShaderCompiler, ShaderCompiler::Plugin);

namespace ShaderCompiler {

ShaderCompilerImplv1_0 Plugin::shaderCompilerImpl;

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
	AssetLoader::RegisterFactory("NFX", ShaderTemplate::Traits::CLASS_ID,
			&Plugin::shaderCompilerImpl);
}

bool Plugin::LicenseExpired() {
	AssetLoader::UnregisterFactory("NFX", ShaderTemplate::Traits::CLASS_ID);
	return true;
}

void Plugin::Dispose() {
}

} /* namespace ShaderCompiler */
