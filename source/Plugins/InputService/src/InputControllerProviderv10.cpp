/*
 * InputControllerProviderv10.cpp
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#include <InputControllerProviderv10.h>

namespace InputService {

InputControllerProviderv1_0::InputControllerProviderv1_0() : baseId(0) {
}

InputControllerProviderv1_0::~InputControllerProviderv1_0() {
}

void InputControllerProviderv1_0::Configure(const Config&) {
}

void InputControllerProviderv1_0::EnumControllers() {
	EnumDevicesImpl();
}

uint32 InputControllerProviderv1_0::GetNumController() {
	return 0;
}

const InputControllerDesc& InputControllerProviderv1_0::GetControllerDesc(
		uint32 n) {
	return InputControllerDesc::Null;
}

InputController* InputControllerProviderv1_0::CreateController(
		uint16 deviceId) {
	return nullptr;
}

void InputControllerProviderv1_0::DestroyController(
		InputController*) {
}

void InputControllerProviderv1_0::SetBaseDevId(uint32 base) {
	baseId = base;
}

} /* namespace InputController */
