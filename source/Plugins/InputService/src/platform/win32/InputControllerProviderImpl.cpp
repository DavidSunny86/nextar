/*
 * InputControllerProviderImpl.cpp
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#include <IncWnds.h>
#include <InputControllerProviderImpl.h>
#include <Win360GamepadController.h>
#include <WinKeyboardMouse.h>
#include <Win32Window.h>

// link to x-input
#if (MSC_VER < 1700) //pre 2012
#	pragma comment(lib,"Xinput.lib")
#else
#	pragma comment(lib,"Xinput9_1_0.lib")
#endif

namespace InputService {
constexpr int32 INVALID_MOUSE_COORD = -99999;
InputControllerProviderImpl::InputControllerProviderImpl() : keyboardAndMouse(nullptr)
,lastMouseX(INVALID_MOUSE_COORD)
,lastMouseY(INVALID_MOUSE_COORD) {
	for (uint32 i = 0; i < 256; ++i)
		virtToKey[i] = KB_UNKNOWN_K;
#define VKEYMAP(m, a) virtToKey[VK_##m] = KB_##a;
#define VKEYMAP_RNAME(a) virtToKey[VK_##a] = KB_##a;
#define VKEYMAP_RANGE(first, last, kf, kl) \
	NEX_STATIC_ASSERT((last - first) == (uint32)((uint32)kl - (uint32)kf)); \
	for (uint32 i = 0; i <= last - first; ++i) \
	virtToKey[i + first] = (KeyName)((uint32)kf + i);

#include <VirtKeyMap.h>
#undef VKEYMAP
#undef VKEYMAP_RANGE
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
	case ControllerType::TYPE_KEYBOARD_AND_MOUSE:
	{
		RAWINPUTDEVICE Rid[2];

		Rid[0].usUsagePage = 0x01;
		Rid[0].usUsage = 0x02;
		Rid[0].dwFlags = RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages
		Rid[0].hwndTarget = 0;

		Rid[1].usUsagePage = 0x01;
		Rid[1].usUsage = 0x06;
		Rid[1].dwFlags = RIDEV_NOLEGACY;   // adds HID keyboard and also ignores legacy keyboard messages
		Rid[1].hwndTarget = 0;

		if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE) {
			Error("Failed to register Keyboard and Mouse!");
			return nullptr;
		}

		return (keyboardAndMouse = NEX_NEW(WinKeyboardMouse(d)));
	}
	case ControllerType::TYPE_XBOX360_CONTROLLER:
		return NEX_NEW(Win360GamepadController(d));
	}
	return nullptr;
}

void InputControllerProviderImpl::DestroyController(InputController* ctrl) {
	WinInputController* c = static_cast<WinInputController*>(ctrl);
	if (c == keyboardAndMouse)
		keyboardAndMouse = nullptr;
	NEX_DELETE(c);
}


void InputControllerProviderImpl::ConsumeEvent(Win32Window* window, HRAWINPUT input) {
	UINT dwSize;
	GetRawInputData(input, RID_INPUT, NULL, &dwSize,
		sizeof(RAWINPUTHEADER));
	rawInputStream.resize(dwSize);
	if (GetRawInputData(input, RID_INPUT, rawInputStream.data(), &dwSize,
		sizeof(RAWINPUTHEADER)) != dwSize)
		Error("GetRawInputData does not return correct size!");

	RAWINPUT* raw = (RAWINPUT*)rawInputStream.data();
	if (raw->header.dwType == RIM_TYPEKEYBOARD && keyboardAndMouse) {
		keyboardAndMouse->AddKeyEvent(virtToKey[raw->data.keyboard.VKey], raw->data.keyboard.Flags & RI_KEY_MAKE);
	} else if (raw->header.dwType == RIM_TYPEMOUSE && keyboardAndMouse) {
		const RAWMOUSE& m = raw->data.mouse;
		if (m.usFlags & MOUSE_MOVE_ABSOLUTE) {
			if (lastMouseX != m.lLastX || lastMouseY != m.lLastY) {
				if (lastMouseX != INVALID_MOUSE_COORD && lastMouseY != INVALID_MOUSE_COORD) {
					//Size s = window->GetCurrentDimensions();
					Size s = Size(1, 1);
					float dx = (float)(m.lLastX - lastMouseX) / (float)s.dx;
					float dy = (float)(m.lLastY - lastMouseY) / (float)s.dy;
					keyboardAndMouse->AddMouseEvent(dx, dy);
				} else {
					lastMouseX = m.lLastX;
					lastMouseY = m.lLastY;
				}
			}
		} else {
			if (m.lLastX || m.lLastY) {
				//Size s = window->GetCurrentDimensions();
				Size s = Size(1, 1);
				float dx = (float)(m.lLastX) / (float)s.dx;
				float dy = (float)(m.lLastY) / (float)s.dy;
				keyboardAndMouse->AddMouseEvent(dx, dy);
			}
		}
		if (m.usButtonFlags) {
			if (m.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
				keyboardAndMouse->AddMouseEvent(KeyName::MOUSE_LEFT, true);
			if (m.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
				keyboardAndMouse->AddMouseEvent(KeyName::MOUSE_LEFT, false);
			if (m.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
				keyboardAndMouse->AddMouseEvent(KeyName::MOUSE_MIDDLE, true);
			if (m.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
				keyboardAndMouse->AddMouseEvent(KeyName::MOUSE_MIDDLE, false);
			if (m.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
				keyboardAndMouse->AddMouseEvent(KeyName::MOUSE_RIGHT, true);
			if (m.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
				keyboardAndMouse->AddMouseEvent(KeyName::MOUSE_RIGHT, false);
			if (m.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
				keyboardAndMouse->AddMouseEvent(KeyName::MOUSE_THUMB1, true);
			if (m.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
				keyboardAndMouse->AddMouseEvent(KeyName::MOUSE_THUMB1, false);
			if (m.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
				keyboardAndMouse->AddMouseEvent(KeyName::MOUSE_THUMB2, true);
			if (m.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
				keyboardAndMouse->AddMouseEvent(KeyName::MOUSE_THUMB2, false);
			if (m.usButtonFlags & RI_MOUSE_WHEEL) {
				int16 data = (int16)m.usButtonData;
				keyboardAndMouse->AddMouseEvent((float)data / (float)WHEEL_DELTA);
			}
		}
	}
}

} /* namespace InputService */
