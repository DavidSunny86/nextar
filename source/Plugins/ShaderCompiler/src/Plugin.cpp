/*
 * Plugin.cpp
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */
#include "BaseHeaders.h"
#include "Plugin.h"

NEX_IMPLEMENT_PLUGIN(ShaderCompiler) {
	return &ShaderCompiler::Plugin::plugin;
}

namespace ShaderCompiler {

	ShaderLoaderImplv1_0 Plugin::shaderLoaderImpl;
	Plugin Plugin::plugin;
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
		ShaderAsset::Loader::RegisterFactory("NFX", &shaderLoaderImpl);
	}

	bool Plugin::LicenseExpired() {
		ShaderAsset::Loader::UnregisterFactory("NFX");
		return true;
	}

	void Plugin::Dispose() {
	}
} /* namespace ShaderCompiler */
