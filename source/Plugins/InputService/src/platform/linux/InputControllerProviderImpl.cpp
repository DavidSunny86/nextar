/*
 * InputControllerProviderImpl.cpp
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#include <InputControllerProviderImpl.h>
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
}

void InputControllerProviderImpl::LookForJoysticks() {
	uint32 index = 0;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[128] = "Unknown";
	do {
		UxDeviceDesc desc;
		String path = devInpPath + "js" + Convert::ToString(index);
		int fd;

		if ((fd = open(path.c_str(), O_RDONLY)) < 0) {
			break;
		}
		ioctl(fd, JSIOCGVERSION, &version);
		ioctl(fd, JSIOCGAXES, &axes);
		ioctl(fd, JSIOCGBUTTONS, &buttons);
		ioctl(fd, JSIOCGNAME(128), name);
		desc.info.name = name;
		desc.info.deviceId = JOYSTICK_DEVID_BASE + index;
		desc.info.type  = GetJoystickType(desc.info.name);
		desc.fd = fd;

		index++;
	} while(true);
}

InputController* InputControllerProviderImpl::CreateController(
		uint16 deviceId) {
	return nullptr;
}

void InputControllerProviderImpl::DestroyController(InputController*) {
}

ControllerType InputControllerProviderImpl::GetJoystickType(const String& name) {
	if (name.find("Microsoft") != String::npos &&
		name.find("X-Box") != String::npos &&
		name.find("360") != String::npos)
		return TYPE_XBOX360_CONTROLLER;

	return TYPE_JOYSTICK;
}


} /* namespace InputService */
