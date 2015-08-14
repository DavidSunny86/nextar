/*
 * UxKeyboardMouse.h
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXKEYBOARDMOUSE_H_
#define PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXKEYBOARDMOUSE_H_

#include <InputController.h>
#include <WinInputController.h>

namespace InputService {

class WinKeyboardMouse: public nextar::DigitalControls,
		public nextar::AnalogControls,
		public WinInputController {
	NEX_LOG_HELPER(WinKeyboardMouse);
public:

	WinKeyboardMouse(const WinDeviceDesc& desc);
	virtual ~WinKeyboardMouse();

	virtual InputChangeBuffer UpdateSettings() override;

	virtual bool IsDown(KeyID);
	virtual bool IsOn(KeyID);
	virtual AnalogValue GetValue(KeyID);
	virtual InputDir GetDir(KeyID);

	virtual DigitalControls* GetDigitalSettings();
	virtual AnalogControls* GetAnalogSettings();

protected:


	bool capsLockState;
	bool numLockState;
	bool scrollLockState;
	uint32 changeCount;
	InputDir mousePos; // absolute pos
	AnalogValue mouseWheel; // absolute value
	array<KeyState, NEX_KEYBOARD_KEY_COUNT>::type  keyboardKeyStates;
	array<KeyState, NEX_MOUSE_DIGITAL_BUTTON_COUNT>::type  mouseButtons;
	array<InputEvent, MAX_CHANGE_BUFFER>::type inputEvents;
};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXKEYBOARDMOUSE_H_ */
