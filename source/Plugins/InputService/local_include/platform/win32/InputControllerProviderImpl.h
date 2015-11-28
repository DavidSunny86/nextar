/*
 * InputControllerProviderImpl.h
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_INPUTCONTROLLERPROVIDERIMPL_H_
#define PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_INPUTCONTROLLERPROVIDERIMPL_H_

#include <Win32InputListener.h>
#include <InputControllerProviderv10.h>
#include <WinInputController.h>

namespace InputService {

class WinKeyboardMouse;
class InputControllerProviderImpl:
	public Win32InputListener,
	public InputService::InputControllerProviderv1_0 {
	NEX_LOG_HELPER(InputControllerProviderImpl);

public:
	InputControllerProviderImpl();
	virtual ~InputControllerProviderImpl();

	virtual void Configure(const Config&);
	virtual void EnumDevicesImpl() override;

	virtual uint32 GetNumController() override {
		return (uint32)controllers.size();
	}

	virtual const InputControllerDesc& GetControllerDesc(uint32 n) {
		return controllers[n]._info;
	}

	const WinDeviceDesc& GetDesc(uint16 deviceId) const;
	virtual InputController* CreateController(uint16 deviceId);
	virtual void DestroyController(InputController*);

	virtual void ConsumeEvent(Win32Window* window, HRAWINPUT);

private:

	enum {
		JOYSTICK_DEVID_BASE = 0x1,
		KEYBOARD_MOUSE_DEVID_BASE = 0x20,
	};

	void FindAndRegisterX360Controller(DWORD index);
	void LookForJoysticks(HANDLE handle);
	void CreateKeyboardAndMouseDesc(HANDLE keyboard, HANDLE mouse);
	
	typedef vector<WinDeviceDesc>::type InputControllerDescList;
	ByteStream rawInputStream;
	WinKeyboardMouse* keyboardAndMouse;
	InputControllerDescList controllers;
	String devInpPath;
	Key virtToKey[256];
	int32 lastMouseX, lastMouseY;
};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_INPUTCONTROLLERPROVIDERIMPL_H_ */
