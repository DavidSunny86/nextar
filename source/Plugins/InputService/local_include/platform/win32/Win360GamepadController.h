/*
 * XBox360Controller.h
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_WIN360CONTROLLER_H_
#define PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_WIN360CONTROLLER_H_

#include <Xinput.h>
#include <WinInputController.h>
#include <InputControllerProviderImpl.h>
#include <TaskSchedular.h>

using namespace nextar;
namespace InputService {

class Win360GamepadController:
		public nextar::DigitalControls,
		public nextar::AnalogControls,
		public WinInputController {
public:
	Win360GamepadController(const WinDeviceDesc& desc);
	virtual ~Win360GamepadController();

	virtual DigitalControls* GetDigitalSettings() override {
		return this;
	}

	virtual AnalogControls* GetAnalogSettings()  override {
		return this;
	}

	virtual InputChangeBuffer UpdateSettings() override;

	virtual bool IsDown(Key) override;
	virtual bool IsOn(Key) override;
	virtual AnalogValue GetValue(Key) override;
	virtual InputDir GetDir(Key) override;

	void InitControls();

	void PollData();

protected:

	static InputDir GetCircular(int32 x, int32 y, int32 deadZone);

	class PollTask : public Task {
	public:
		PollTask();

		
		inline void Lock() {
			while (lock_.test_and_set(std::memory_order_relaxed));
		}
		inline bool TryLock() {
			return !lock_.test_and_set(std::memory_order_relaxed);
		}
		inline void Unlock() {
			lock_.clear();
		}
		inline void SetDevice(Win360GamepadController* dev) {
			device_ = dev;
		}

		virtual Task* Run();

	protected:
		Win360GamepadController* device_;
		atomic_flag lock_;
	};

	template <typename T>
	struct KeyValue {
		T value;
	};

	typedef KeyValue<KeyState> KeyStateType;
	typedef KeyValue<AnalogValue>	AnalogValueType;
	typedef KeyValue<InputDir> InputDirType;
	typedef array<InputEvent, MAX_CHANGE_BUFFER>::type InputEventList;

	bool enabled;
	bool currBuffer;

	KeyStateType buttonStates[NEX_XB360_CTRL_BUTTON_END - NEX_XB360_CTRL_BUTTON_START];
	AnalogValueType trigValues[2];
	InputDirType axes[2];
	AnalogValue prevDirButtonState[2];

	XINPUT_STATE prevState;
	PollTask pollTask;
	DWORD index;
	uint32 changeCount;
	InputEventList inputEvents[2];

	int32 thumbDeadZone[2];
	int32 triggerDeadZone;

	static Key buttonMap[16];
};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_WIN360CONTROLLER_H_ */
