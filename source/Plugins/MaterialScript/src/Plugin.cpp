/*
 * Plugin.cpp
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */
#include <MaterialScript.h>
#include <BaseHeaders.h>
#include <Plugin.h>

NEX_IMPLEMENT_PLUGIN(MaterialScript, MaterialScript::Plugin);

namespace MaterialScript {

MaterialScriptImplv1_0 Plugin::materialCompilerImpl;

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
	Trace("Loaded material script 1.0.");
}

void Plugin::Close() {
	Trace("Unloading material script 1.0.");
}

void Plugin::LicenseRenewed() {
	materialCompilerImpl.RegisterDictionary();
	AssetLoader::RegisterFactory("MSCRIPT", MaterialTemplate::Traits::CLASS_ID,
			&materialCompilerImpl);
}

bool Plugin::LicenseExpired() {
	AssetLoader::UnregisterFactory("MSCRIPT", MaterialTemplate::Traits::CLASS_ID);
	materialCompilerImpl.UnregisterDictionary();
	return true;
}

void Plugin::Dispose() {
}
} /* namespace ShaderCompiler */
