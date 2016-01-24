/*
 * XBox360Controller.h
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_XBOX360CONTROLLER_H_
#define PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_XBOX360CONTROLLER_H_

#include <UxInputController.h>
#include <InputControllerProviderImpl.h>
#include <TaskSchedular.h>

struct js_event;
using namespace nextar;
namespace InputService {

class Ux360GamepadController:
		public nextar::DigitalControls,
		public nextar::AnalogControls,
		public UxInputController {
public:
	Ux360GamepadController(const UxDeviceDesc& desc);
	virtual ~Ux360GamepadController();

	virtual DigitalControls* GetDigitalSettings() override {
		return this;
	}

	virtual AnalogControls* GetAnalogSettings()  override {
		return this;
	}

	virtual InputChangeBuffer UpdateSettings() override;

	virtual bool IsDown(KeyName) override;
	virtual bool IsOn(KeyName) override;
	virtual AnalogValue GetValue(KeyName) override;
	virtual InputDir GetDir(KeyName) override;

	void InitControls();

protected:

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
		inline void SetDevice(Ux360GamepadController* dev) {
			device_ = dev;
		}

		virtual Task* Run();

	protected:
		Ux360GamepadController* device_;
		atomic_flag lock_;
	};

	void PollData();
	void ParseAxis(const js_event& ev, bool init);
	void ParseButton(const js_event& ev, bool init);
	void ParseData(const js_event& ev, bool init);

	typedef vector<KeyName>::type KeyMap;
	template <typename T>
	struct KeyValue {
		T value;
		uint32 timeStamp;
	};

	enum Constants : int16 {
		MAX_CHANGE_BUFFER = 32,
		VAL_MAX = 32767
	};

	typedef KeyValue<KeyState> KeyStateType;
	typedef KeyValue<AnalogValue> AnalogValueType;
	typedef KeyValue<InputDir> InputDirType;

	array<KeyName, 9>::type reverseButtonMap;
	array<KeyName, 8>::type reverseAxisMap;

	bool currBuffer;
	PollTask pollTask;
	KeyStateType buttonStates[NEX_XB360_CTRL_BUTTON_END-NEX_XB360_CTRL_BUTTON_START];
	AnalogValueType trigValues[2];
	InputDirType axes[2];
	AnalogValue prevDirButtonState[2];

	int32 thumbDeadZone[2];
	int32 triggerDeadZone;

	uint32 changeCount;
	typedef array<InputEvent, MAX_CHANGE_BUFFER>::type InputEventList;
	InputEventList inputEvents[2];
};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_XBOX360CONTROLLER_H_ */
