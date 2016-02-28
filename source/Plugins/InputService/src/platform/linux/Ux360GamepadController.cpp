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
#include <Ux360GamepadController.h>
#include <KeyName.h>

namespace InputService {

Ux360GamepadController::PollTask::PollTask() : device_(nullptr) {
}

Task* Ux360GamepadController::PollTask::Run() {
	device_->PollData();
	return nullptr;
}

Ux360GamepadController::Ux360GamepadController(const UxDeviceDesc& desc) :
	UxInputController(desc), changeCount(0), currBuffer(0) {
	InitControls();
	pollTask.SetDevice(this);
	TaskSchedular::Instance().AsyncSubmit(&pollTask);
}

Ux360GamepadController::~Ux360GamepadController() {
	pollTask.Wait();
	close(fd);
}

InputChangeBuffer Ux360GamepadController::UpdateSettings() {
	// see if task was executed
	InputChangeBuffer buffer(0, 0);
	if (pollTask.IsCompleted()) {
		buffer.first = inputEvents[currBuffer].data();
		buffer.second = changeCount;
		currBuffer = !currBuffer;
		TaskSchedular::Instance().AsyncSubmit(&pollTask);
		//pollTask.Run();
	}
	return buffer;
}

bool Ux360GamepadController::IsDown(KeyName k) {
	NEX_ASSERT(k >= NEX_XB360_CTRL_BUTTON_START && k < NEX_XB360_CTRL_BUTTON_END);
	return (bool)buttonStates[k - NEX_XB360_CTRL_BUTTON_START].value;
}

bool Ux360GamepadController::IsOn(KeyName k) {
	return false;
}

void Ux360GamepadController::PollData() {
	size_t readSize = 0;
	js_event js;

	changeCount = 0;
	while ((readSize = read(fd, &js, sizeof(struct js_event)))
			== sizeof(struct js_event))  {
		if (!(js.type & JS_EVENT_INIT) && changeCount < MAX_CHANGE_BUFFER) {
			ParseData(js, false);
		}
	}
}

AnalogValue Ux360GamepadController::GetValue(KeyName k) {
	NEX_ASSERT(XBOX_TRIG_LEFT == k || XBOX_TRIG_RIGHT == k);
	return trigValues[k-XBOX_TRIG_LEFT].value;
}

InputDir Ux360GamepadController::GetDir(KeyName k) {
	NEX_ASSERT(XBOX_AXIS_LEFT == k || XBOX_AXIS_RIGHT == k);
	return axes[k - XBOX_AXIS_LEFT].value;
}

void Ux360GamepadController::ParseAxis(const js_event& js, bool init) {
	InputEventList& iel = inputEvents[currBuffer];
	if (js.number >= 0 && js.number < reverseAxisMap.size()) {
		KeyName k = reverseAxisMap[js.number];
		switch(k) {
		case XBOX_AXIS_RIGHT:
		case XBOX_AXIS_LEFT: {
			int hand = 0;
			int axis = (js.number % 2);
			int sign = 1;
			if (k == XBOX_AXIS_RIGHT) {
				hand = 1;
				axis = !axis;
			}

			if ( axis )
				sign = -1;

			int32 deadzone = thumbDeadZone[hand];
			int32 absVal = std::abs(js.value);
			if (absVal < deadzone) {
				axes[hand].value.xy[axis] = 0;
			} else {
				int32 value = (absVal - deadzone) * Math::Sign<int16>(js.value) * sign;
				axes[hand].value.xy[axis] = (float)value / (float)(VAL_MAX - deadzone);
			}

			axes[hand].timeStamp = js.time;
			if (!init) {
				iel[changeCount].analogDir = axes[hand].value;
				iel[changeCount].timeStamp = js.time;
				iel[changeCount].key = k;
				changeCount++;
			}
		}
		break;
		case XBOX_TRIG_LEFT:
		case XBOX_TRIG_RIGHT: {
			int hand = (k == XBOX_TRIG_RIGHT);
			int32 absVal = std::abs(js.value);
			if (absVal < triggerDeadZone) {
				trigValues[hand].value = 0;
				init = true;
			} else {
				int32 value = (absVal - triggerDeadZone) * Math::Sign<int16>(js.value);
				trigValues[hand].value = (float)value / (float)(VAL_MAX - triggerDeadZone);
			}

			axes[hand].timeStamp = js.time;
			if (!init) {
				iel[changeCount].analogValue = trigValues[hand].value;
				iel[changeCount].timeStamp = js.time;
				iel[changeCount].key = k;
				changeCount++;
			}
		}
		break;
		case XBOX_LEFT:
		case XBOX_UP: {
			KeyName key = XBOX_UP;
			KeyState state = KeyState::KEY_STATE_DOWN;
			int axis = (k == XBOX_UP);
			// if its a key up event
			if (js.value == 0) {
				if (prevDirButtonState[axis]) {
					state = KeyState::KEY_STATE_UP;
					key = (KeyName)(((prevDirButtonState[axis] < 0) ? XBOX_LEFT : XBOX_RIGHT) + axis*2);
				} else
					init = true;
			} else {
				key = (KeyName)(((js.value < 0) ? XBOX_LEFT : XBOX_RIGHT) + axis*2);
			}

			if (!init) {
				iel[changeCount].keyState = state;
				iel[changeCount].timeStamp = js.time;
				iel[changeCount].key = key;
				changeCount++;
			}
			prevDirButtonState[axis] = js.value;
		}
		break;
		}
	}
}

void Ux360GamepadController::ParseButton(const js_event& js, bool init) {
	InputEventList& iel = inputEvents[currBuffer];
	if (js.number >= 0 && js.number < reverseButtonMap.size()) {
		KeyName k = reverseButtonMap[js.number];
		KeyState state = js.value == 0? KeyState::KEY_STATE_UP : KeyState::KEY_STATE_DOWN;
		buttonStates[k - NEX_XB360_CTRL_BUTTON_START].value = state;
		buttonStates[k - NEX_XB360_CTRL_BUTTON_START].timeStamp = js.time;
		if (!init) {
			iel[changeCount].keyState = state;
			iel[changeCount].timeStamp = js.time;
			iel[changeCount].key = k;
			changeCount++;
		}
	}
}

void Ux360GamepadController::ParseData(const js_event& js, bool init) {
	if(js.type & JS_EVENT_AXIS) {
		ParseAxis(js, init);
	}
	if(js.type & JS_EVENT_BUTTON) {
		ParseButton(js, init);
	}
}

void Ux360GamepadController::InitControls() {
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
	// @urgent Missing XBOX_LEFT_THUMB & XBOX_RIGHT_THUMB

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

	for(uint i = 0; i < 2; ++ i) {
		auto& events = inputEvents[i];
		for(auto& e : events) {
			e.deviceId = desc.deviceId;
			e.timeStamp = 0;
		}
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
