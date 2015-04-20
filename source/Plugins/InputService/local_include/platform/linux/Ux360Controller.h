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

struct js_event;
using namespace nextar;
namespace InputService {

class Ux360Controller:
		public nextar::DigitalControls,
		public nextar::AnalogControls,
		public UxInputController {
public:
	Ux360Controller(const UxDeviceDesc& desc);
	virtual ~Ux360Controller();

	virtual DigitalControls* GetDigitalSettings() override {
		return this;
	}

	virtual AnalogControls* GetAnalogSettings()  override {
		return this;
	}

	virtual InputChangeBuffer UpdateSettings() override;
	virtual void PollData() override;

	virtual bool IsDown(KeyID) override;
	virtual bool IsOn(KeyID) override;
	virtual AnalogValue GetValue(KeyID) override;
	virtual InputDir GetDir(KeyID) override;

	void InitControls();

protected:

	void ParseAxis(const js_event& ev, bool init);
	void ParseButton(const js_event& ev, bool init);
	void ParseData(const js_event& ev, bool init);

	typedef vector<KeyID>::type KeyMap;
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

	array<KeyID, 9>::type reverseButtonMap;
	array<KeyID, 8>::type reverseAxisMap;

	KeyStateType buttonStates[NEX_XB360_CTRL_BUTTON_END-NEX_XB360_CTRL_BUTTON_START];
	AnalogValueType trigValues[2];
	InputDirType axes[2];
	AnalogValue prevDirButtonState[2];

	AnalogValue thumbDeadZone[2];
	AnalogValue triggerDeadZone;

	uint32 changeCount;
    array<InputEvent, MAX_CHANGE_BUFFER>::type inputEvents;
};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_XBOX360CONTROLLER_H_ */
