/*
 * Plugin.cpp
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#include <Plugin.h>

namespace InputService {

NEX_IMPLEMENT_PLUGIN(InputService, InputService::Plugin);


Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
	Trace("Loaded input service 1.0.");
}

void Plugin::Close() {
	Trace("Unloading input service 1.0.");
}

void Plugin::LicenseRenewed() {
}

bool Plugin::LicenseExpired() {
}

void Plugin::Dispose() {
}

} /* namespace InputController */
