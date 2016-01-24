/* 
 * File:   InputManager.h
 * Author: obhi
 *
 * Created on June 10, 2011
 */
#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <KeyName.h>

namespace nextar {

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

	bool IsValid() const {
		return (type != TYPE_UNKNOWN) && (deviceId != (uint16)-1);
	}

	static const InputControllerDesc Null;
};

/* ranges between 0 and 1*/
typedef float AnalogValue;

struct InputDir {
	// x and y of delta
	AnalogValue xy[2];
};

struct InputEvent {
	uint16 deviceId;
	KeyName key;
	uint32 timeStamp;
	union {
		KeyState keyState;
		AnalogValue analogValue;
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

	virtual void SetProperty(const String& propertyName, const String& value) {}

	const InputControllerDesc& GetDesc() const {
		return desc;
	}

protected:
	~InputController() {
	}

	InputControllerDesc desc;
};

class AnalogControls {
public:
	virtual AnalogValue GetValue(KeyName) = 0;
	virtual InputDir GetDir(KeyName) = 0;
protected:
	~AnalogControls() {
	}
};

class DigitalControls {
public:
	virtual bool IsDown(KeyName) = 0;
	/* Valid for toggle keys */
	virtual bool IsOn(KeyName) = 0;
protected:
	~DigitalControls() {
	}
};

namespace InputUtils {

inline bool IsAnalog(KeyName key) {
	return (key & KEY_ANALOG_BIT) != 0;
}

inline bool IsDigital(KeyName key) {
	return (key & (KEY_ANALOG_BIT | KEY_DIRECTION_BIT)) == 0;
}

inline bool IsDirection(KeyName key) {
	return (key & KEY_DIRECTION_BIT) == 0;
}

_NexEngineAPI int32 GetCookedCode(KeyName, bool shift, bool numlockon);

}

}

#endif //INPUTCONTROLLER_H
