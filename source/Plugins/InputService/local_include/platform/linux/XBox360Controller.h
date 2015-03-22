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

using namespace nextar;
namespace InputService {

class XBox360Controller:
		public nextar::DigitalControls,
		public nextar::AnalogControls,
		public UxInputController {
public:
	XBox360Controller(const UxDeviceDesc& desc);
	virtual ~XBox360Controller();

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
	virtual int32 GetValue(KeyID) override;
	virtual InputDir GetDir(KeyID) override;

protected:
    InputEvent inputEvents[32];
};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_XBOX360CONTROLLER_H_ */
