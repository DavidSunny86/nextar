/*
 * InputControllerProviderImpl.h
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_INPUTCONTROLLERPROVIDERIMPL_H_
#define PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_INPUTCONTROLLERPROVIDERIMPL_H_

#include <InputControllerProviderv10.h>
#include <UxInputController.h>

namespace InputService {


class InputControllerProviderImpl:
		public InputService::InputControllerProviderv1_0 {
public:
	InputControllerProviderImpl();
	virtual ~InputControllerProviderImpl();

	virtual void Configure(const Config&);
	virtual void EnumDevicesImpl() override;

	virtual uint32 GetNumController() override {
		return controllers.size();
	}

	virtual const InputControllerDesc& GetControllerDesc(uint32 n) {
		return controllers[n].info;
	}

	virtual InputController* CreateController(uint16 deviceId);
	virtual void DestroyController(InputController*);

private:

	enum {
		JOYSTICK_DEVID_BASE = 0x0100,
		KEYBOARD_MOUSE_DEVID_BASE = 0x0200,
	};

	void LookForJoysticks();

	ControllerType GetJoystickType(const String& name);


	typedef vector<UxDeviceDesc>::type InputControllerDescList;

	InputControllerDescList controllers;
	String devInpPath;
};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_INPUTCONTROLLERPROVIDERIMPL_H_ */
