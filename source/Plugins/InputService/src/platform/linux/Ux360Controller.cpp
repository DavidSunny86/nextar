/*
 * XBox360Controller.cpp
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/joystick.h>
#include <Ux360Controller.h>
#include <KeyCode.h>

namespace InputService {

Ux360Controller::PollTask::PollTask() : device_(nullptr),
	lock_(ATOMIC_FLAG_INIT) {
	lock_.clear();
}

Task* Ux360Controller::PollTask::Run() {
	device_->PollData();
	Unlock();
}

Ux360Controller::Ux360Controller(const UxDeviceDesc& desc) :
	UxInputController(desc), changeCount(0), currBuffer(0) {
	InitControls();
	pollTask.SetDevice(this);
	TaskSchedular::Instance().AsyncSubmit(&pollTask);
}

Ux360Controller::~Ux360Controller() {
}

InputChangeBuffer Ux360Controller::UpdateSettings() {
	// see if task was executed
	InputChangeBuffer buffer(0, 0);
	if (pollTask.TryLock()) {
		buffer.first = inputEvents[currBuffer].data();
		buffer.second = changeCount;
		currBuffer = !currBuffer;
		pollTask.Unlock();
		TaskSchedular::Instance().AsyncSubmit(&pollTask);
	}
	return buffer;
}

bool Ux360Controller::IsDown(KeyID keyId) {
	NEX_ASSERT(keyId >= NEX_XB360_CTRL_BUTTON_START && keyId < NEX_XB360_CTRL_BUTTON_END);
	return (bool)buttonStates[keyId - NEX_XB360_CTRL_BUTTON_START].value;
}

bool Ux360Controller::IsOn(KeyID keyId) {
	return false;
}

void Ux360Controller::PollData() {
	size_t readSize = 0;
	js_event js;

	changeCount = 0;
	while ((readSize = read(fd, &js, sizeof(struct js_event)))
			== sizeof(struct js_event))  {
		if (!(js.type & JS_EVENT_INIT)) {
			ParseData(js, changeCount < MAX_CHANGE_BUFFER);
		}
	}
}

AnalogValue Ux360Controller::GetValue(KeyID keyId) {
	NEX_ASSERT(XBOX_TRIG_LEFT == keyId || XBOX_TRIG_RIGHT == keyId);
	return trigValues[keyId-XBOX_TRIG_LEFT].value;
}

InputDir Ux360Controller::GetDir(KeyID keyId) {
	NEX_ASSERT(XBOX_AXIS_LEFT == keyId || XBOX_AXIS_RIGHT == keyId);
	return axes[keyId - XBOX_AXIS_LEFT].value;
}

void Ux360Controller::ParseAxis(const js_event& js, bool init) {
	if (js.number >= 0 && js.number < reverseAxisMap.size()) {
		KeyID k = reverseAxisMap[js.number];
		switch(k) {
		case XBOX_AXIS_RIGHT:
		case XBOX_AXIS_LEFT: {
			int hand = (k == XBOX_AXIS_RIGHT);
			int axis = js.number & 2;
			int32 deadzone = thumbDeadZone[hand];
			AnalogValue absVal = std::abs(js.value);
			if (absVal < deadzone) {
				axes[hand].value.xy[axis] = 0;
				init = true;
			} else {
				int32 value = (absVal - deadzone) * Math::Sign<int16>(js.value) *
						VAL_MAX;
				axes[hand].value.xy[axis] = value / (VAL_MAX-deadzone);
			}

			axes[hand].timeStamp = js.time;
			if (!init) {
				inputEvents[changeCount].analogDir = axes[hand].value;
				inputEvents[changeCount].timeStamp = js.time;
				inputEvents[changeCount].key = k;
				changeCount++;
			}
		}
		break;
		case XBOX_TRIG_LEFT:
		case XBOX_TRIG_RIGHT: {
			int hand = (k == XBOX_TRIG_RIGHT);
			AnalogValue absVal = std::abs(js.value);
			if (absVal < triggerDeadZone) {
				trigValues[hand].value = 0;
				init = true;
			} else {
				int32 value = (absVal - triggerDeadZone) * Math::Sign<int16>(js.value) *
						VAL_MAX;
				trigValues[hand].value = value / (VAL_MAX-triggerDeadZone);
			}

			axes[hand].timeStamp = js.time;
			if (!init) {
				inputEvents[changeCount].analogValue = trigValues[hand].value;
				inputEvents[changeCount].timeStamp = js.time;
				inputEvents[changeCount].key = k;
				changeCount++;
			}
		}
		break;
		case XBOX_LEFT:
		case XBOX_UP: {
			KeyID key = XBOX_UP;
			KeyState state = KeyState::KEY_STATE_DOWN;
			int axis = (k == XBOX_UP);
			// if its a key up event
			if (js.value == 0) {
				if (prevDirButtonState[axis]) {
					state = KeyState::KEY_STATE_UP;
					key = ((prevDirButtonState[axis] < 0) ? XBOX_LEFT : XBOX_RIGHT) + axis*2;
				} else
					init = true;
			} else {
				key = ((js.value < 0) ? XBOX_LEFT : XBOX_RIGHT) + axis*2;
			}

			if (!init) {
				inputEvents[changeCount].keyState = state;
				inputEvents[changeCount].timeStamp = js.time;
				inputEvents[changeCount].key = key;
				changeCount++;
			}
			prevDirButtonState[axis] = js.value;
		}
		break;
		}
	}
}

void Ux360Controller::ParseButton(const js_event& js, bool init) {
	if (js.number >= 0 && js.number < reverseButtonMap.size()) {
		KeyID k = reverseButtonMap[js.number];
		KeyState state = js.value == 0? KeyState::KEY_STATE_UP : KeyState::KEY_STATE_DOWN;
		buttonStates[k - NEX_XB360_CTRL_BUTTON_START].value = state;
		buttonStates[k - NEX_XB360_CTRL_BUTTON_START].timeStamp = js.time;
		if (!init) {
			inputEvents[changeCount].keyState = state;
			inputEvents[changeCount].timeStamp = js.time;
			inputEvents[changeCount].key = k;
			changeCount++;
		}
	}
}

void Ux360Controller::ParseData(const js_event& js, bool init) {
	if(js.type & JS_EVENT_AXIS) {
		ParseAxis(js, init);
	}
	if(js.type & JS_EVENT_BUTTON) {
		ParseButton(js, init);
	}
}

void Ux360Controller::InitControls() {
	// setup default map
	reverseButtonMap[0] = XBOX_A;
	reverseButtonMap[1] = XBOX_B;
	reverseButtonMap[2] = XBOX_X;
	reverseButtonMap[3] = XBOX_Y;
	reverseButtonMap[4] = XBOX_SHOULDER_BUTTON_LEFT;
	reverseButtonMap[5] = XBOX_SHOULDER_BUTTON_RIGHT;
	reverseButtonMap[6] = XBOX_BACK;
	reverseButtonMap[7] = XBOX_START;
	reverseButtonMap[8] = XBOX_CONTROLLER_SEL;

	reverseAxisMap[0] = XBOX_AXIS_LEFT;
	reverseAxisMap[1] = XBOX_AXIS_LEFT;
	reverseAxisMap[2] = XBOX_TRIG_LEFT;
	reverseAxisMap[3] = XBOX_AXIS_RIGHT;
	reverseAxisMap[4] = XBOX_AXIS_RIGHT;
	reverseAxisMap[5] = XBOX_TRIG_RIGHT;
	reverseAxisMap[6] = XBOX_LEFT;
	reverseAxisMap[7] = XBOX_UP;

	thumbDeadZone[0] = 7849;
	thumbDeadZone[1] = 8689;
	triggerDeadZone = 30;

	for(auto& e : inputEvents) {
		e.deviceId = desc.deviceId;
		e.timeStamp = 0;
	}
	size_t readSize = 0;
	js_event js;
	fcntl(fd, F_SETFL, O_NONBLOCK);
	while ((readSize = read(fd, &js, sizeof(struct js_event)))
				== sizeof(struct js_event))  {
		if (js.type & JS_EVENT_INIT) {
			ParseData(js, true);
		}
	}
}

} /* namespace InputService */
