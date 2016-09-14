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

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
	Trace("Loading resource packer version 1.0");
}

void Plugin::Close() {
	Trace("Unloading resource packer version 1.0");
}

void Plugin::LicenseRenewed() {
}

bool Plugin::LicenseExpired() {
	return true;
}

void Plugin::Dispose() {
}

} /* namespace ResourcePacker */
