/*
 * UxKeyboardMouse.h
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXKEYBOARDMOUSE_H_
#define PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXKEYBOARDMOUSE_H_

#include <InputController.h>
#include <UxInputController.h>
#include <XWinInputListener.h>

namespace InputService {

class UxKeyboardMouse: public nextar::DigitalControls,
		public nextar::AnalogControls,
		public UxInputController {
	NEX_LOG_HELPER(UxKeyboardMouse);
public:

	UxKeyboardMouse(const UxDeviceDesc& desc);
	virtual ~UxKeyboardMouse();

	virtual InputChangeBuffer UpdateSettings() override;

	virtual bool IsDown(KeyName);
	virtual bool IsOn(KeyName);
	virtual AnalogValue GetValue(KeyName);
	virtual InputDir GetDir(KeyName);

	virtual DigitalControls* GetDigitalSettings();
	virtual AnalogControls* GetAnalogSettings();

protected:
	class Listener : public XWinInputListener {
	public:
		typedef unordered_map<KeySym, nextar::KeyName>::type KeySymMap;
		typedef array<uint32, 256>::type KeyCodeToSymMap;
		Listener();
		~Listener();

		void SetDevice(UxKeyboardMouse* device_) {
			device = device_;
		}

		virtual void ConsumeKeyEvent(nextar::XWindow* window,
				XEvent& event, bool pressEvent);
		virtual void ConsumeMouseMove(nextar::XWindow* window,
				XEvent& event);
		virtual void ConsumeMouseButtonEvent(nextar::XWindow* window,
				XEvent& event, bool pressEvent);

	protected:
		void DetermineLockKeyStates(Display* dpy);
		void RetrieveKeyLayout(Display* dpy);
		bool ReadKeyMap(Display* dpy, const URL&);
		bool ReadKeySymMap(const URL&, KeySymMap& ksm);
		KeyCodeToSymMap GenerateKeyboardMapping(Display* dpy, const KeySymMap& ksm);
		void WriteKeyboardMapping(const URL& path, const KeyCodeToSymMap&);

		int minKeyCode;
		int maxKeyCode;
		array<KeyName, 256>::type keyCodeToKey;
		bool keyCodesInited;
		UxKeyboardMouse* device;
	};

public:
	inline void SetKbKeyState(KeyName k, KeyState s) {
		keyboardKeyStates[k] = s;
	}

	inline InputEvent* PushEvent() {
		if (changeCount < MAX_CHANGE_BUFFER)
			return &inputEvents[changeCount++];
		return nullptr;
	}

	inline void SetCapsLock(bool l) {
		capsLockState = l;
	}

	inline void SetNumLock(bool l) {
		numLockState = l;
	}

	inline void SetScrollLock(bool l) {
		scrollLockState = l;
	}

	inline void SetMouseLoc(const InputDir& i) {
		mousePos = i;
	}

	enum Constants : int16 {
		MAX_CHANGE_BUFFER = 64,
		INVALID_ANALOG_VAL = 32767,
	};

protected:

	Listener listener;

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
