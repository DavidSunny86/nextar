/*
 * Plugin.cpp
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */
#include <CommonTypes.h>
#include <BaseHeaders.h>
#include <Plugin.h>

NEX_IMPLEMENT_PLUGIN(MaterialCompiler, MaterialCompiler::Plugin);

namespace MaterialCompiler {

MaterialCompilerImplv1_0 Plugin::materialCompilerImpl;

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
	Trace("Loaded material compiler 1.0.");
}

void Plugin::Close() {
	Trace("Unloading material compiler 1.0.");
}

void Plugin::LicenseRenewed() {
	AssetLoader::RegisterFactory("MTL", MaterialTemplate::Traits::CLASS_ID,
			&materialCompilerImpl);
}

bool Plugin::LicenseExpired() {
	AssetLoader::UnregisterFactory("MTL", MaterialTemplate::Traits::CLASS_ID);
	return true;
}

void Plugin::Dispose() {
}
} /* namespace ShaderCompiler */
