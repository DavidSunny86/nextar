/*
 * InputControllerProviderImpl.cpp
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#include <IncWnds.h>
#include <InputControllerProviderImpl.h>
#include <Win360GamepadController.h>


// link to x-input
#if (MSC_VER < 1700) //pre 2012
#	pragma comment(lib,"Xinput.lib")
#else
#	pragma comment(lib,"Xinput9_1_0.lib")
#endif

namespace InputService {

InputControllerProviderImpl::InputControllerProviderImpl() {
	// TODO Auto-generated constructor stub

}

InputControllerProviderImpl::~InputControllerProviderImpl() {
	// TODO Auto-generated destructor stub
}

void InputControllerProviderImpl::Configure(const Config& config) {
	InputControllerProviderv1_0::Configure(config);
}

void InputControllerProviderImpl::EnumDevicesImpl() {
		
	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) { 
		Error("Failed to obtain RAW Input list!");
		NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	}
	
	pRawInputDeviceList = (PRAWINPUTDEVICELIST)NEX_ALLOC(sizeof(RAWINPUTDEVICELIST) * nDevices, MEMCAT_GENERAL);

	if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1) { 
		Error("Failed to obtain RAW devices!");
		NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	}
	// do the job...
	HANDLE keyboard = 0;
	HANDLE mouse = 0;
	for (uint32 i = 0; i < nDevices; ++i) {
		if (pRawInputDeviceList[i].dwType == RIM_TYPEKEYBOARD)
			keyboard = pRawInputDeviceList[i].hDevice;
		else if (pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE)
			mouse = pRawInputDeviceList[i].hDevice;
		else
			LookForJoysticks(pRawInputDeviceList[i].hDevice);
	}

	// check xbox 360 controllers
	for (DWORD index = 0; index < 4; ++index) {
		FindAndRegisterX360Controller(index);
	}
	
	if (keyboard && mouse)
		CreateKeyboardAndMouseDesc(keyboard, mouse);

	NEX_FREE(pRawInputDeviceList, MEMCAT_GENERAL);
}

void InputControllerProviderImpl::FindAndRegisterX360Controller(DWORD index) {
	XINPUT_CAPABILITIES caps;
	if (XInputGetCapabilities(index, 0, &caps) == ERROR_SUCCESS) {
		if (caps.Type == XINPUT_DEVTYPE_GAMEPAD &&
			caps.SubType == XINPUT_DEVSUBTYPE_GAMEPAD) {
			// xbox360 gamepad
			String name = "XBox 360 Gamepad Controller: " + Convert::ToString((uint32)index);
			controllers.push_back(WinDeviceDesc(index, name, (uint16)controllers.size() + baseId, ControllerType::TYPE_XBOX360_CONTROLLER));
		}
	}
}

void InputControllerProviderImpl::CreateKeyboardAndMouseDesc(HANDLE keyboard,
	HANDLE mouse) {
	LookForJoysticks(keyboard);
	LookForJoysticks(mouse);
	this->controllers.push_back(WinDeviceDesc(keyboard, mouse, "Keyboard & Mouse", (uint16)controllers.size() + baseId));
}

void InputControllerProviderImpl::LookForJoysticks(HANDLE hand) {
	/*char deviceName[512];
	UINT nameLen = 512;
	RID_DEVICE_INFO deviceInfo;

	GetRawInputDeviceInfo(hand, RIDI_DEVICENAME, deviceName, &nameLen);
	nameLen = sizeof(deviceInfo);
	GetRawInputDeviceInfo(hand, RIDI_DEVICEINFO, &deviceInfo, &nameLen);*/
}

const WinDeviceDesc& InputControllerProviderImpl::GetDesc(uint16 deviceId) const {
	for (auto& e : controllers) {
		if (e._info.deviceId == deviceId)
			return e;
	}
	return WinDeviceDesc::Null;
}

InputController* InputControllerProviderImpl::CreateController(
		uint16 deviceId) {
	const WinDeviceDesc& d = GetDesc(deviceId);
	switch (d._info.type) {
	case ControllerType::TYPE_XBOX360_CONTROLLER:
		return NEX_NEW(Win360GamepadController(d));
	}
	return nullptr;
}

void InputControllerProviderImpl::DestroyController(InputController* ctrl) {
	WinInputController* c = static_cast<WinInputController*>(ctrl);
	NEX_DELETE(c);
}


} /* namespace InputService */
