/*
 * XBox360Controller.cpp
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#include <WinInputController.h>
#include <Win360GamepadController.h>

namespace InputService {

Key Win360GamepadController::buttonMap[16] = { (Key)0 };
Win360GamepadController::PollTask::PollTask() : Task("PollXBox360"), device_(nullptr) {
	lock_.clear();
}

Task* Win360GamepadController::PollTask::Run() {
	device_->PollData();
	Unlock();
	return nullptr;
}

Win360GamepadController::Win360GamepadController(const WinDeviceDesc& desc) :
WinInputController(desc), index(desc._xInputIndex) {
	currBuffer = false;
	enabled = true;
	changeCount = 0;
	pollTask.SetDevice(this);
	thumbDeadZone[0] = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	thumbDeadZone[1] = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
	triggerDeadZone = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

	InitControls();
}

Win360GamepadController::~Win360GamepadController() {
	pollTask.Wait();
}

void Win360GamepadController::PollData() {
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(state));
	changeCount = 0;

	DWORD dwResult = XInputGetState(index, &state);
	DWORD timeStamp = GetTickCount();
	if (dwResult == ERROR_SUCCESS) {
		if (state.dwPacketNumber != prevState.dwPacketNumber) {
			// we have some changes we will need to register
			InputEventList& iel = inputEvents[currBuffer];
			
			if (state.Gamepad.wButtons != prevState.Gamepad.wButtons) {
				
				// insert button events
#define XBUTSTATE(state, i) (state.Gamepad.wButtons & (1 << i))

				for (uint32 i = 0; i < 16; ++i) {
					if (XBUTSTATE(state, i) != XBUTSTATE(prevState, i)) {
						if (buttonMap[i] != Key::KEY_INVALID_CODE) {
							InputEvent &ev = iel[changeCount++];
							KeyState k = buttonStates[buttonMap[i] - NEX_XB360_CTRL_BUTTON_START].value = XBUTSTATE(state, i) ? KeyState::KEY_STATE_DOWN : KeyState::KEY_STATE_UP;
							ev.key = buttonMap[i];
							ev.keyState = k;
							ev.timeStamp = timeStamp;
						}
					}
				}

#undef XBUTSTATE
			}

			if (state.Gamepad.bLeftTrigger != prevState.Gamepad.bLeftTrigger) {
				InputEvent &ev = iel[changeCount++];
				if (state.Gamepad.bLeftTrigger < triggerDeadZone)
					state.Gamepad.bLeftTrigger = 0;
				else
					state.Gamepad.bLeftTrigger -= triggerDeadZone;
				ev.analogValue = trigValues[0].value = (float)state.Gamepad.bLeftTrigger / (255.0f - triggerDeadZone);
				ev.key = Key::XBOX_TRIG_LEFT;
				ev.timeStamp = timeStamp;
				
			}

			if (state.Gamepad.bRightTrigger != prevState.Gamepad.bRightTrigger) {
				InputEvent &ev = iel[changeCount++];
				if (state.Gamepad.bRightTrigger < triggerDeadZone)
					state.Gamepad.bRightTrigger = 0;
				else
					state.Gamepad.bRightTrigger -= triggerDeadZone;
				ev.analogValue = trigValues[1].value = (float)state.Gamepad.bRightTrigger / (255.0f - triggerDeadZone);
				ev.key = Key::XBOX_TRIG_RIGHT;
				ev.timeStamp = timeStamp;
			}
						
			if (state.Gamepad.sThumbLX != prevState.Gamepad.sThumbLX ||
				state.Gamepad.sThumbLY != prevState.Gamepad.sThumbLY) {
				InputEvent &ev = iel[changeCount++];
				ev.analogDir = axes[0].value = GetCircular(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, thumbDeadZone[0]);
				ev.key = Key::XBOX_AXIS_LEFT;
				ev.timeStamp = timeStamp;
			}

			if (state.Gamepad.sThumbRX != prevState.Gamepad.sThumbRX ||
				state.Gamepad.sThumbRY != prevState.Gamepad.sThumbRY) {
				InputEvent &ev = iel[changeCount++];
				ev.analogDir = axes[0].value = GetCircular(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY, thumbDeadZone[1]);
				ev.key = Key::XBOX_AXIS_RIGHT;
				ev.timeStamp = timeStamp;
			}
			prevState = state;
		}
	}
}

InputDir Win360GamepadController::GetCircular(int32 x, int32 y, 
	int32 deadZone) {
	

	float fx, fy;

	//determine how far the controller is pushed
	int32 absX = std::abs(x);
	int32 absY = std::abs(y);

	if (absX > deadZone) {
		int32 value = (absX - deadZone) * Math::Sign<int16>(x);
		fx = (float)value / (float)(32767 - deadZone);
	} else
		fx = 0;

	if (absY > deadZone) {
		int32 value = (absY - deadZone) * Math::Sign<int16>(y);
		fy = (float)value / (float)(32767 - deadZone);
	} else
		fy = 0;

	InputDir d = { fx, fy };
	return d;
}

InputChangeBuffer Win360GamepadController::UpdateSettings() {
	// see if task was executed
	InputChangeBuffer buffer(0, 0);
	if (pollTask.TryLock()) {
		PollData();
		buffer.first = inputEvents[currBuffer].data();
		buffer.second = changeCount;
		currBuffer = !currBuffer;
		TaskSchedular::Instance().AsyncSubmit(&pollTask);
	}

	return buffer;
}

bool Win360GamepadController::IsDown(Key Key) {
	NEX_ASSERT(Key >= NEX_XB360_CTRL_BUTTON_START && Key < NEX_XB360_CTRL_BUTTON_END);
	return false;
	//return (bool)buttonStates[Key - NEX_XB360_CTRL_BUTTON_START].value;
}

bool Win360GamepadController::IsOn(Key Key) {
	return false;
}

AnalogValue Win360GamepadController::GetValue(Key Key) {
	NEX_ASSERT(XBOX_TRIG_LEFT == Key || XBOX_TRIG_RIGHT == Key);
	return 0;
}

InputDir Win360GamepadController::GetDir(Key Key) {
	NEX_ASSERT(XBOX_AXIS_LEFT == Key || XBOX_AXIS_RIGHT == Key);
	return InputDir();
}

void Win360GamepadController::InitControls() {
	// setup default map

	if (buttonMap[0] == (Key)0 ) {
		for (int i = 0; i < 16; ++i)
			buttonMap[i] = Key::KEY_INVALID_CODE;

		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_DPAD_UP>::position] = Key::XBOX_UP;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_DPAD_DOWN>::position] = Key::XBOX_DOWN;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_DPAD_LEFT>::position] = Key::XBOX_LEFT;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_DPAD_RIGHT>::position] = Key::XBOX_RIGHT;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_START>::position] = Key::XBOX_START;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_BACK>::position] = Key::XBOX_BACK;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_LEFT_THUMB>::position] = Key::XBOX_LEFT_THUMB;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_RIGHT_THUMB>::position] = Key::XBOX_RIGHT_THUMB;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_LEFT_SHOULDER>::position] = Key::XBOX_SHOULDER_BUTTON_LEFT;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_RIGHT_SHOULDER>::position] = Key::XBOX_SHOULDER_BUTTON_RIGHT;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_A>::position] = Key::XBOX_A;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_B>::position] = Key::XBOX_B;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_X>::position] = Key::XBOX_X;
		buttonMap[Utils::first_set_bit_pos<XINPUT_GAMEPAD_Y>::position] = Key::XBOX_Y;
	}

	DWORD dwResult = XInputGetState(index, &prevState);
	if (dwResult != ERROR_SUCCESS)
		enabled = false;
	
	for(uint32 i = 0; i < 2; ++ i) {
		auto& events = inputEvents[i];
		for(auto& e : events) {
			e.deviceId = desc.deviceId;
			e.timeStamp = 0;
		}
	}
}

} /* namespace InputService */
