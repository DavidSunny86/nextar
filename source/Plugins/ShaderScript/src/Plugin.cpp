/*
 * Plugin.cpp
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */
#include <CommonTypes.h>
#include <BaseHeaders.h>
#include <Plugin.h>

NEX_IMPLEMENT_PLUGIN(ShaderScript, ShaderScript::Plugin);

namespace ShaderScript {

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
	Trace("Loaded shader script 1.0.");
}

void Plugin::Close() {
	Trace("Unloading shader script 1.0.");
}

void Plugin::LicenseRenewed() {
	impl.RegisterDictionary();
	AssetLoader::RegisterFactory("FXSCRIPT", ShaderTemplate::Traits::CLASS_ID,
			&impl);
}

bool Plugin::LicenseExpired() {
	AssetLoader::UnregisterFactory("FXSCRIPT", ShaderTemplate::Traits::CLASS_ID);
	impl.UnregisterDictionary();
	return true;
}

void Plugin::Dispose() {
}
} /* namespace ShaderCompiler */
