/*
 * Plugin.cpp
 *
 *  Created on: Sep 18, 2016
 *      Author: obhi
 */

#include <Plugin.h>
#include <ResourcePackerServiceImpl.h>

NEX_IMPLEMENT_PLUGIN(ResourcePacker, ResourcePacker::Plugin);

namespace ResourcePacker {

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
	Trace("Loaded resource packer 1.0.");
}

void Plugin::Close() {
	if (_packerServiceImpl.IsInUse())
		NEX_THROW_FatalError(EXCEPT_UNLOAD_FAILED_DUE_TO_DANGLING_REF);
	Trace("Unloading resource packer 1.0.");
}

void Plugin::LicenseRenewed() {
}

bool Plugin::LicenseExpired() {
	return true;
}

void Plugin::Dispose() {
}

PluginService* Plugin::Query(const char* name) {
	if (_packerServiceImpl.GetMeta().IsNamed(name)) {
		_packerServiceImpl.IncrementUsage();
		return &_packerServiceImpl;
	}

	return nullptr;
}
} /* namespace ResourcePacker */
