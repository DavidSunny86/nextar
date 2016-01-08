/*
 * WinKeyboardMouse.cpp
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#include <WinKeyboardMouse.h>
#include <WindowManager.h>

namespace InputService {

/**
 * @todo
 */


/***************************************************
 * WinKeyboardMouse
 ***************************************************/
WinKeyboardMouse::WinKeyboardMouse(const WinDeviceDesc& d) :
WinInputController(d), changeCount(0), 
scrollLockState(false), capsLockState(false), numLockState(false),
mouseWheel(0) {
	mousePos = { { 0, 0 } };
	std::fill(keyboardKeyStates.begin(), keyboardKeyStates.end(), KEY_STATE_UP);
	std::fill(mouseButtons.begin(), mouseButtons.end(), KEY_STATE_UP);
	mouseDisplacement.xy[0] = 0;
	mouseDisplacement.xy[1] = 0;
}

WinKeyboardMouse::~WinKeyboardMouse() {

}

void WinKeyboardMouse::AddKeyEvent(Key code, bool bDown) {
	if (changeCount >= MAX_CHANGE_BUFFER) {
		Warn("Too many input events!");
		return;
	}
	InputEvent& ie = inputEvents[changeCount++];
	ie.key = code;
	ie.keyState = (KeyState)bDown;
	keyboardKeyStates[code - NEX_KEYBOARD_KEY_START] = ie.keyState;
}

void WinKeyboardMouse::AddMouseEvent(Key code, bool bDown) {
	CommitMouseMoveEvent();
	if (changeCount >= MAX_CHANGE_BUFFER) {
		Warn("Too many input events!");
		return;
	}
	InputEvent& ie = inputEvents[changeCount++];
	ie.key = code;
	ie.keyState = (KeyState)bDown;
	mouseButtons[code - NEX_MOUSE_BUTTON_START] = ie.keyState;
}

void WinKeyboardMouse::AddMouseEvent(float x, float y) {
	mouseDisplacement.xy[0] += x;
	mouseDisplacement.xy[1] += y;
}

void WinKeyboardMouse::AddMouseEvent(float wheelValue) {
	CommitMouseMoveEvent();
	if (changeCount >= MAX_CHANGE_BUFFER) {
		Warn("Too many input events!");
		return;
	}
	InputEvent& ie = inputEvents[changeCount++];
	ie.key = Key::MOUSE_WHEEL;
	ie.analogValue = wheelValue;
	mouseWheel += wheelValue;
}

void WinKeyboardMouse::CommitMouseMoveEvent() {
	if (mouseDisplacement.xy[0] || mouseDisplacement.xy[1]) {
		if (changeCount >= MAX_CHANGE_BUFFER) {
			Warn("Too many input events!");
		}
		else {
			InputEvent& ie = inputEvents[changeCount++];
			ie.key = Key::MOUSE_XY_AXIS;
			ie.analogDir.xy[0] = mouseDisplacement.xy[0];
			ie.analogDir.xy[1] = mouseDisplacement.xy[1];
			mousePos.xy[0] += mouseDisplacement.xy[0];
			mousePos.xy[1] += mouseDisplacement.xy[1];
			mouseDisplacement.xy[0] = 0;
			mouseDisplacement.xy[1] = 0;
		}
	}
}

InputChangeBuffer WinKeyboardMouse::UpdateSettings() {
	CommitMouseMoveEvent();
	InputChangeBuffer buffer;
	buffer.first = inputEvents.data();
	buffer.second = changeCount;
	changeCount = 0;
	// reset mouse position
	if (lockMouseToCenter) {
		RenderWindow* rw = WindowManager::Instance().GetActiveWindow();
		if (rw) {
			Size s = rw->GetCurrentDimensions();
			SetCursorPos(s.dx >> 1, s.dy >> 1);
		}
	}
	return buffer;
}

bool WinKeyboardMouse::IsDown(Key Key) {
	if (Key >= NEX_KEYBOARD_KEY_START && Key <= NEX_KEYBOARD_KEY_END)
		return keyboardKeyStates[Key - NEX_KEYBOARD_KEY_START] == KeyState::KEY_STATE_DOWN;
	else if (Key >= NEX_MOUSE_BUTTON_START && Key <= NEX_MOUSE_BUTTON_END)
		return mouseButtons[Key - NEX_MOUSE_BUTTON_START] == KeyState::KEY_STATE_DOWN;
	return false;
}

bool WinKeyboardMouse::IsOn(Key k) {
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

AnalogValue WinKeyboardMouse::GetValue(Key k) {
	if (k == Key::MOUSE_WHEEL)
		return mouseWheel;
	return 0;
}

InputDir WinKeyboardMouse::GetDir(Key k) {
	if (k == Key::MOUSE_XY_AXIS)
		return mousePos;
	return InputDir();
}

DigitalControls* WinKeyboardMouse::GetDigitalSettings() {
	return this;
}

AnalogControls* WinKeyboardMouse::GetAnalogSettings() {
	return this;
}

void WinKeyboardMouse::SetProperty(const String& propertyName,
	const String& value) {
	if (propertyName == "lock_cursor")
		lockMouseToCenter = value == "1" || value == "true" ? true : false;
}

} /* namespace InputService */

