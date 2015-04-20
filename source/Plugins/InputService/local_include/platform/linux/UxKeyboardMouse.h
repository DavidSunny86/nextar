/*
 * UxKeyboardMouse.h
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXKEYBOARDMOUSE_H_
#define PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXKEYBOARDMOUSE_H_

#include <InputController.h>
#include <UxInputController.h>

namespace InputService {

class UxKeyboardMouse: public nextar::DigitalControls,
		public nextar::AnalogControls,
		public UxInputController {
public:
	UxKeyboardMouse(const UxDeviceDesc& desc);
	virtual ~UxKeyboardMouse();
};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_LINUX_UXKEYBOARDMOUSE_H_ */
