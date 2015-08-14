/*
 * WinInputController.h
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_WIN32_WININPUTCONTROLLER_H_
#define PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_WIN32_WININPUTCONTROLLER_H_

#include <InputController.h>

namespace InputService {

using namespace nextar;
struct WinDeviceDesc {
	
	InputControllerDesc _info;
	HANDLE _device;
	DWORD _xInputIndex;
	HANDLE _keyboard;
	HANDLE _mouse;
	
	WinDeviceDesc() { _device = 0; _keyboard = 0; _mouse = 0; _xInputIndex = -1; }
	WinDeviceDesc(const String& name, uint16 deviceId, ControllerType type) : WinDeviceDesc() {
		_info.name = name;
		_info.deviceId = deviceId;
		_info.type = type;
	}

	WinDeviceDesc(DWORD xInputIndex, const String& name, uint16 deviceId, ControllerType type) : WinDeviceDesc(name, deviceId, type) {
		_xInputIndex = xInputIndex;
	}

	WinDeviceDesc(HANDLE device, const String& name, uint16 deviceId, ControllerType type) : WinDeviceDesc(name, deviceId, type) {
		_device = device;
	}

	WinDeviceDesc(HANDLE keyboard, HANDLE mouse, const String& name, uint16 deviceId) : WinDeviceDesc(name, deviceId, ControllerType::TYPE_KEYBOARD_AND_MOUSE) {
		_keyboard = keyboard;
		_mouse = mouse;
	}

	WinDeviceDesc(const WinDeviceDesc& desc) : 
		WinDeviceDesc(desc._info.name, desc._info.deviceId, desc._info.type) {
		_keyboard = desc._keyboard;
		_mouse = desc._mouse;
		_xInputIndex = desc._xInputIndex;
	}

	static const WinDeviceDesc Null;

	bool IsValid() const {
		return _info.IsValid();
	}
};

class WinInputController:
		public AllocGeneral,
		public nextar::InputController {
public:
	enum {
		MAX_CHANGE_BUFFER = 32
	};
	WinInputController(const WinDeviceDesc& desc);
	virtual ~WinInputController();

	virtual InputChangeBuffer UpdateSettings() = 0;

protected:
	uint32 axes;
	uint32 buttons;
	int fd;
};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_WIN32_WININPUTCONTROLLER_H_ */
