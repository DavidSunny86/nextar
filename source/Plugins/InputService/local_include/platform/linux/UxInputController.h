/*
 * UxInputController.h
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXINPUTCONTROLLER_H_
#define PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXINPUTCONTROLLER_H_

#include <InputController.h>

namespace InputService {

using namespace nextar;
struct UxDeviceDesc {
	InputControllerDesc info;
	int fd;
	uint32 axes;
	uint32 buttons;

	UxDeviceDesc() : fd(-1), axes(0), buttons(0) {}

	static const UxDeviceDesc Null;

	bool IsValid() const {
		return fd != Null.fd && info.IsValid();
	}
};

class UxInputController:
		public AllocGeneral,
		public nextar::InputController {
public:
	UxInputController(const UxDeviceDesc& desc);
	virtual ~UxInputController();

	virtual void PollData() = 0;

protected:
	int fd;

};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXINPUTCONTROLLER_H_ */
