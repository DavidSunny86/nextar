/* 
 * File:   InputManager.h
 * Author: obhi
 *
 * Created on June 10, 2011
 */
#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <KeyCode.h>

namespace nextar {

enum KeyState {
	KEY_STATE_UP,
	KEY_STATE_DOWN
};

enum KeyAnalogBit {
	KEY_ANALOG_BIT = 0x8000,
	KEY_DIRECTION_BIT = 0x4000
};

typedef uint16 KeyID;

enum ControllerType : uint16 {
	TYPE_KEYBOARD_AND_MOUSE,
	TYPE_XBOX360_CONTROLLER,
	TYPE_JOYSTICK,
	TYPE_UNKNOWN,
};

struct _NexEngineAPI InputControllerDesc {
	String name;
	ControllerType type;
	uint16 deviceId;

	InputControllerDesc() : type(TYPE_UNKNOWN), deviceId(-1) {}
	InputControllerDesc(const InputControllerDesc& d) : name(d.name), type(d.type),
			deviceId(d.deviceId) {}

	bool IsValid() {
		return (type != TYPE_UNKNOWN) && (deviceId != (uint16)-1);
	}

	static const InputControllerDesc Null;
};

struct InputDir {
	uint16 deltaX;
	uint16 deltaY;
};

struct InputEvent {
	uint16 deviceId;
	KeyID key;
	uint32 timeStamp;
	union {
		KeyState keyState;
		int32 analogValue;
		InputDir analogDir;
	};
};

typedef std::pair<const InputEvent*, uint32> InputChangeBuffer;

class _NexEngineAPI InputListener {
public:
	virtual void ProcessEvents(const InputChangeBuffer&) = 0;
protected:
	~InputListener() {
	}
};

class AnalogControls;
class DigitalControls;

/** @todo Add active/inactive, used/unused support */
class _NexEngineAPI InputController {
public:
	InputController() {}
	InputController(const InputControllerDesc& _desc) : desc(_desc) {}

	virtual DigitalControls* GetDigitalSettings() = 0;
	virtual AnalogControls* GetAnalogSettings() = 0;

	virtual InputChangeBuffer UpdateSettings() = 0;

	const InputControllerDesc& GetDesc() const {
		return desc;
	}

protected:
	~InputController() {
	}

private:
	InputControllerDesc desc;
};

class AnalogControls {
public:
	virtual int32 GetValue(KeyID) = 0;
	virtual InputDir GetDir(KeyID) = 0;
protected:
	~AnalogControls() {
	}
};

class DigitalControls {
public:
	virtual bool IsDown(KeyID) = 0;
	/* Valid for toggle keys */
	virtual bool IsOn(KeyID) = 0;
protected:
	~DigitalControls() {
	}
};

namespace InputUtils {

inline bool IsAnalog(KeyID key) {
	return (key & KEY_ANALOG_BIT) != 0;
}

inline bool IsDigital(KeyID key) {
	return (key & (KEY_ANALOG_BIT | KEY_DIRECTION_BIT)) == 0;
}

inline bool IsDirection(KeyID key) {
	return (key & KEY_DIRECTION_BIT) == 0;
}

_NexEngineAPI int32 GetCookedCode(KeyID, bool shift, bool numlockon);

}

}

#endif //INPUTCONTROLLER_H
