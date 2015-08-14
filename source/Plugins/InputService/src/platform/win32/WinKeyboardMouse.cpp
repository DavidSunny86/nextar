/*
 * WinKeyboardMouse.cpp
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#include <WinKeyboardMouse.h>

namespace InputService {

/**
 * @todo
 */


/***************************************************
 * WinKeyboardMouse
 ***************************************************/
WinKeyboardMouse::WinKeyboardMouse(const WinDeviceDesc& d) :
	WinInputController(d), changeCount(0), mousePos(),
	scrollLockState(false), capsLockState(false), numLockState(false),
	mouseWheel(0) {
	std::fill(keyboardKeyStates.begin(), keyboardKeyStates.end(), KEY_STATE_UP);
	std::fill(mouseButtons.begin(), mouseButtons.end(), KEY_STATE_UP);
}

WinKeyboardMouse::~WinKeyboardMouse() {

}

InputChangeBuffer WinKeyboardMouse::UpdateSettings() {
	InputChangeBuffer buffer;
	buffer.first = inputEvents.data();
	buffer.second = changeCount;
	changeCount = 0;
	return buffer;
}

bool WinKeyboardMouse::IsDown(KeyID keyId) {
	if (keyId >= NEX_KEYBOARD_KEY_START && keyId <= NEX_KEYBOARD_KEY_END)
		return keyboardKeyStates[keyId - NEX_KEYBOARD_KEY_START] == KeyState::KEY_STATE_DOWN;
	else if (keyId >= NEX_MOUSE_BUTTON_START && keyId <= NEX_MOUSE_BUTTON_END)
		return mouseButtons[keyId - NEX_MOUSE_BUTTON_START] == KeyState::KEY_STATE_DOWN;
	return false;
}

bool WinKeyboardMouse::IsOn(KeyID k) {
	switch (k) {
	case Key::KB_CAPITAL:
		return capsLockState;
	case Key::KB_NUMLOCK:
		return numLockState;
	case Key::KB_SCROLL:
		return scrollLockState;
	}
	return false;
}

AnalogValue WinKeyboardMouse::GetValue(KeyID k) {
	return 0;
}

InputDir WinKeyboardMouse::GetDir(KeyID unsignedShortInt) {
	return InputDir();
}

DigitalControls* WinKeyboardMouse::GetDigitalSettings() {
	return this;
}

AnalogControls* WinKeyboardMouse::GetAnalogSettings() {
	return this;
}

} /* namespace InputService */

