/*
 * UxInputController.h
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXINPUTCONTROLLER_H_
#define PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXINPUTCONTROLLER_H_

#include <InputController.h>

struct UxDeviceDesc {
	InputControllerDesc info;
	int fd;
};

namespace InputService {

class UxInputController: public nextar::InputController {
public:
	UxInputController(const UxDeviceDesc& desc);
	virtual ~UxInputController();

	virtual void PollData() = 0;

protected:
	int fd;

};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXINPUTCONTROLLER_H_ */
