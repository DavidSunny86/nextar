/*
 * InputControllerProviderImpl.cpp
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#include <InputControllerProviderImpl.h>
#include <UxKeyboardMouse.h>
#include <Ux360GamepadController.h>
#include <linux/joystick.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

namespace InputService {

InputControllerProviderImpl::InputControllerProviderImpl() {
	// TODO Auto-generated constructor stub

}

InputControllerProviderImpl::~InputControllerProviderImpl() {
	// TODO Auto-generated destructor stub
}

void InputControllerProviderImpl::Configure(const Config& config) {
	InputControllerProviderv1_0::Configure(config);
	devInpPath = config.GetValue("InputDevices", "Linux_DevInputPath", "/dev/input/");
}

void InputControllerProviderImpl::EnumDevicesImpl() {
	LookForJoysticks();
	CreateKeyboardAndMouseDesc();
}

void InputControllerProviderImpl::CreateKeyboardAndMouseDesc() {
	UxDeviceDesc desc;
	// @todo If we have keyboard
	desc.info.name = "Keyboard and Mouse";
	desc.info.deviceId = baseId + KEYBOARD_MOUSE_DEVID_BASE + 1;
	desc.info.type = TYPE_KEYBOARD_AND_MOUSE;
	desc.fd = -2;
	// @todo Buttons and Axes count
	desc.buttons = 0;
	desc.axes = 0;
	controllers.push_back(desc);
}

void InputControllerProviderImpl::LookForJoysticks() {
	uint32 index = 0;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[128] = "Unknown";
	UxDeviceDesc desc;
	do {

		String path = devInpPath + "js" + Convert::ToString(index);
		int fd;

		if ((fd = open(path.c_str(), O_RDONLY|O_NONBLOCK)) < 0) {
			break;
		}
		ioctl(fd, JSIOCGVERSION, &version);
		ioctl(fd, JSIOCGAXES, &axes);
		ioctl(fd, JSIOCGBUTTONS, &buttons);
		ioctl(fd, JSIOCGNAME(128), name);
		desc.info.name = name;
		desc.info.deviceId = baseId + JOYSTICK_DEVID_BASE + index;
		desc.info.type  = GetJoystickType(desc.info.name, axes, buttons, version);
		desc.fd = UxDeviceDesc::NULL_FD;
		desc.axes = axes;
		desc.buttons = buttons;

		controllers.push_back(desc);
		index++;
	} while(true);
}

const UxDeviceDesc& InputControllerProviderImpl::GetDesc(uint16 deviceId) const {
	for(auto& c : controllers) {
		if (c.info.deviceId == deviceId) {
			return c;
		}
	}
	return UxDeviceDesc::Null;
}

InputController* InputControllerProviderImpl::CreateController(
		uint16 deviceId) {
	const UxDeviceDesc& desc = GetDesc(deviceId);
	if (desc.IsValid()) {
		switch(desc.info.type) {
		case TYPE_XBOX360_CONTROLLER:
			return NEX_NEW(Ux360GamepadController(desc));
		case TYPE_KEYBOARD_AND_MOUSE:
			return NEX_NEW(UxKeyboardMouse(desc));
		}
	}
	return nullptr;
}

void InputControllerProviderImpl::DestroyController(InputController* ctrl) {
	UxInputController* ctrl_ = static_cast<UxInputController*>(ctrl);
	NEX_DELETE(ctrl_);
}

ControllerType InputControllerProviderImpl::GetJoystickType(const String& name, uint32 axes,
		uint32 buttons, uint32 version) {
	if (name.find("Microsoft") != String::npos &&
		name.find("X-Box") != String::npos &&
		name.find("360") != String::npos) {

		return TYPE_XBOX360_CONTROLLER;
	}

	return TYPE_JOYSTICK;
}


} /* namespace InputService */
