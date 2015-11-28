/*
 * UxKeyboardMouse.cpp
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#include <UxKeyboardMouse.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>

namespace InputService {

/**
 * @todo
 * #include <X11/keysym.h>
 * Check that header file for keysyms
 * The display is required here, we wait for message proc to send us an event for init
 * While initing, use XGetKeyboardMapping to generate the mapping from keycode->keysym->Key
 * The info should be enough to build a map from keycode->Key and we use that map in key events.
 */

/***************************************************
 * UxKeyboardMouse::Listener
 ***************************************************/
UxKeyboardMouse::Listener::Listener() :
	keyCodesInited(false), minKeyCode(0), maxKeyCode(0), device(nullptr) {
	for(auto& k : keyCodeToKey)
		k = Key::KEY_INVALID_CODE;
}

UxKeyboardMouse::Listener::~Listener() {
}

void UxKeyboardMouse::Listener::DetermineLockKeyStates(Display* dpy) {
	unsigned int state;
	if (XkbGetIndicatorState(dpy, XkbUseCoreKbd, &state) == Success) {
		device->SetCapsLock(((state & 1) == 1));
		device->SetNumLock(((state & 2) == 2));
		device->SetScrollLock(((state & 4) == 4));
	} else {
		device->SetCapsLock(false);
		device->SetNumLock(false);
		device->SetScrollLock(false);
	}
}

void UxKeyboardMouse::Listener::WriteKeyboardMapping(const URL& path,
		const UxKeyboardMouse::Listener::KeyCodeToSymMap& m) {
	OutputStreamPtr ostr = FileSystem::Instance().OpenWrite(path, false);
	OutputSerializer oser(ostr);
	VersionID ver = NEX_MAKE_VERSION(1,0,1);
	String version = Convert::ToVersionString(ver);
	String name = "UxKeyCodeMap";
	oser << name << version;
	uint32 minKC = (uint32)minKeyCode;
	uint32 maxKC = (uint32)maxKeyCode;
	oser << minKC << maxKC;
	OutputSerializer::UShortArray a((uint16*)keyCodeToKey.data(), 256);
	OutputSerializer::UIntArray b((uint32*)m.data(), 256);
	oser << a << b;
}

bool UxKeyboardMouse::Listener::ReadKeyMap(Display* dpy, const URL& path) {
	// read stuff
	XDisplayKeycodes(dpy, &minKeyCode, &maxKeyCode);
	int perKeycodeReturn;
	NEX_ASSERT(minKeyCode >= 0 && maxKeyCode < 256);
	InputStreamPtr istr = FileSystem::Instance().OpenRead(path);
	if (!istr)
		return false;
	InputSerializer iser(istr);
	String version, name;
	iser >> name >> version;
	uint32 minKC = 257;
	uint32 maxKC = 0;
	if(!TestVersion(NEX_MAKE_VERSION(1,0,1), Convert::ToVersion(version))) {
		return false;
	}
	iser >> minKC >> maxKC;
	if (minKC != minKeyCode || maxKC != maxKeyCode)
		return false;

	KeyCodeToSymMap m;
	InputSerializer::UShortArray a((uint16*)keyCodeToKey.data(), 256);
	InputSerializer::UIntArray b((uint32*)m.data(), 256);
	iser >> a >> b;

	KeySym* allSyms;
	allSyms = XGetKeyboardMapping(dpy, minKeyCode, ((maxKeyCode+1) - minKeyCode),
					&perKeycodeReturn);
	bool ok = true;
	for(uint32 i = minKeyCode; i < maxKeyCode; ++i) {
#define G_SYM(n, i) allSyms[(((n-minKeyCode)*perKeycodeReturn + i))]
		KeySym s = G_SYM(i, 0);
		if (s != m[i]) {
			ok = false;
			break;
		}
#undef G_SYM
	}

	if (allSyms)
		XFree(allSyms);
	return ok;
}

UxKeyboardMouse::Listener::KeyCodeToSymMap
UxKeyboardMouse::Listener::GenerateKeyboardMapping(Display* dpy,
		const KeySymMap& ksm) {

	KeyCodeToSymMap retMap;

	XDisplayKeycodes(dpy, &minKeyCode, &maxKeyCode);

	int perKeycodeReturn;
	KeySym* allSyms;
	allSyms = XGetKeyboardMapping(dpy, minKeyCode, ((maxKeyCode+1) - minKeyCode),
				&perKeycodeReturn);

	NEX_ASSERT(minKeyCode >= 0 && maxKeyCode < 256);
#define G_SYM(n, i) allSyms[(((n-minKeyCode)*perKeycodeReturn + i))]
	for(int i = minKeyCode; i < maxKeyCode; ++i) {
		KeySym s = G_SYM(i, 0);
		retMap[i] = s;
		auto it = ksm.find(s);
		if (it != ksm.end())
			keyCodeToKey[i] = (*it).second;
	}
#undef G_SYM
	if (allSyms)
		XFree(allSyms);

	return retMap;
}

bool UxKeyboardMouse::Listener::ReadKeySymMap(const URL& path, KeySymMap& ksm) {
	InputStreamPtr istr = FileSystem::Instance().OpenRead(path);
	if (istr) {
		InputSerializer iser(istr);
		String name, version;
		iser >> name >> version;
		if(TestVersion(NEX_MAKE_VERSION(1,0,1), Convert::ToVersion(version))) {
			uint32 keySym[NEX_KEYBOARD_KEY_COUNT] = {0};
			InputSerializer::UIntArray arr(keySym, NEX_KEYBOARD_KEY_COUNT);
			iser >> arr;
			for(uint32 i = 0; i < NEX_KEYBOARD_KEY_COUNT; ++i) {
				if (keySym[i]) {
					ksm.emplace((KeySym)keySym[i], (Key)i);
				}
			}
			return true;
		}
	}
	return false;
}

void UxKeyboardMouse::Listener::RetrieveKeyLayout(Display* dpy) {
	Config& c = ApplicationContext::Instance().GetConfig();
	String keyMap = c.GetValue("InputDevices", "Linux_KeyMap", StringUtils::Null);
	URL kmpath;
	if (!keyMap.length())
		kmpath = URL(FileSystem::ArchiveEngineData_Name, "Configs/UxKeyMap.dat");
	else
		kmpath = URL(keyMap);
	if (!ReadKeyMap(dpy, kmpath)) {
		URL path;
		// we have not yet generated this section
		String keySyms = c.GetValue("InputDevices", "Linux_KeySymMap", StringUtils::Null);
		KeySymMap keySymMap;
		if (!keySyms.length())
			path = URL(FileSystem::ArchiveEngineData_Name, "Configs/UxKeySyms.dat");
		else
			path = URL(keySyms);
		if (!ReadKeySymMap(path, keySymMap)) {
			Error("Problem with configuration file, no key-sym map file!");
			NEX_THROW_FatalError(EXCEPT_DEVICE_CREATION_FAILED);
		}
		auto m = GenerateKeyboardMapping(dpy, keySymMap);
		WriteKeyboardMapping(kmpath, m);
	}
	keyCodesInited = true;
}

void UxKeyboardMouse::Listener::ConsumeKeyEvent(nextar::XWindow* window, XEvent& event,
		bool pressEvent) {
	if (!keyCodesInited)
		RetrieveKeyLayout(window->GetDisplay());

	Key id = keyCodeToKey[event.xkey.keycode];
	KeyState state = pressEvent? KeyState::KEY_STATE_DOWN : KeyState::KEY_STATE_UP;
	device->SetKbKeyState(id, state);
	InputEvent* inpEvent = device->PushEvent();
	if (id == Key::KB_SCROLL || id == Key::KB_CAPITAL ||
			id == Key::KB_NUMLOCK)
		DetermineLockKeyStates(window->GetDisplay());

	if (inpEvent) {
		inpEvent->key = id;
		inpEvent->keyState = state;
		inpEvent->timeStamp = event.xkey.time;
	}
}

void UxKeyboardMouse::Listener::ConsumeMouseMove(nextar::XWindow* window, XEvent& event) {
	InputDir inp = {static_cast<AnalogValue>(event.xmotion.x),
			static_cast<AnalogValue>(event.xmotion.y)};
	device->SetMouseLoc(inp);
	InputEvent* inpEvent = device->PushEvent();
	if (inpEvent) {
		inpEvent->key = Key::MOUSE_XY_AXIS;
		inpEvent->analogDir = inp;
		inpEvent->timeStamp = event.xmotion.time;
	}
}

void UxKeyboardMouse::Listener::ConsumeMouseButtonEvent(nextar::XWindow* window,
		XEvent& event, bool pressEvent) {
	if(event.xbutton.button == 4 || event.xbutton.button == 5) {
		device->mouseWheel += (((event.xbutton.button - 4) << 1)-1);
		InputEvent* inpEvent = device->PushEvent();
		if (inpEvent) {
			inpEvent->key = Key::MOUSE_WHEEL;
			inpEvent->analogValue = device->mouseWheel;
			inpEvent->timeStamp = event.xbutton.time;
		}
	} else {
		Key k = (event.xbutton.button-Button1) + Key::MOUSE_LEFT;
		KeyState state = device->mouseButtons[k] = (pressEvent ? KeyState::KEY_STATE_DOWN : KeyState::KEY_STATE_UP);
		InputEvent* inpEvent = device->PushEvent();
		if (inpEvent) {
			inpEvent->key = k;
			inpEvent->keyState = state;
			inpEvent->timeStamp = event.xbutton.time;
		}
	}
}

/***************************************************
 * UxKeyboardMouse
 ***************************************************/
UxKeyboardMouse::UxKeyboardMouse(const UxDeviceDesc& d) :
	UxInputController(d), changeCount(0), mousePos{INVALID_ANALOG_VAL,INVALID_ANALOG_VAL},
	scrollLockState(false), capsLockState(false), numLockState(false),
	mouseWheel(0) {
	listener.SetDevice(this);
	std::fill(keyboardKeyStates.begin(), keyboardKeyStates.end(), KEY_STATE_UP);
	std::fill(mouseButtons.begin(), mouseButtons.end(), KEY_STATE_UP);
}

UxKeyboardMouse::~UxKeyboardMouse() {

}

InputChangeBuffer UxKeyboardMouse::UpdateSettings() {
	InputChangeBuffer buffer;
	buffer.first = inputEvents.data();
	buffer.second = changeCount;
	changeCount = 0;
	return buffer;
}

bool UxKeyboardMouse::IsDown(Key Key) {
	if (Key >= NEX_KEYBOARD_KEY_START && Key <= NEX_KEYBOARD_KEY_END)
		return keyboardKeyStates[Key - NEX_KEYBOARD_KEY_START] == KeyState::KEY_STATE_DOWN;
	else if (Key >= NEX_MOUSE_BUTTON_START && Key <= NEX_MOUSE_BUTTON_END)
		return mouseButtons[Key - NEX_MOUSE_BUTTON_START] == KeyState::KEY_STATE_DOWN;
	return false;
}

bool UxKeyboardMouse::IsOn(Key k) {
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

AnalogValue UxKeyboardMouse::GetValue(Key k) {
	return 0;
}

InputDir UxKeyboardMouse::GetDir(Key unsignedShortInt) {
	return InputDir();
}

DigitalControls* UxKeyboardMouse::GetDigitalSettings() {
	return this;
}

AnalogControls* UxKeyboardMouse::GetAnalogSettings() {
	return this;
}

} /* namespace InputService */

