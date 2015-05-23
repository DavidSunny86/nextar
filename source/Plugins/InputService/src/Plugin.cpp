/*
 * Plugin.cpp
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#include <Plugin.h>
#include <InputManager.h>

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
	if (InputManager::InstancePtr())
		InputManager::Instance().RegisterProvider(&_impl);
}

bool Plugin::LicenseExpired() {
	if (InputManager::InstancePtr())
		InputManager::Instance().UnregisterProvider(&_impl);
	return true;
}

void Plugin::Dispose() {
}

} /* namespace InputController */
