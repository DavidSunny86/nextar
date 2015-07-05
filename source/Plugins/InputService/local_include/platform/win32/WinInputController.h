/*
 * WinInputController.h
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_WIN32_WININPUTCONTROLLER_H_
#define PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_WIN32_WININPUTCONTROLLER_H_

#include <InputController.h>

namespace InputService {

using namespace nextar;
struct WinDeviceDesc {
	
	InputControllerDesc info;
	
	WinDeviceDesc() {}

	static const WinDeviceDesc Null;

	bool IsValid() const {
		return info.IsValid();
	}
};

class WinInputController:
		public AllocGeneral,
		public nextar::InputController {
public:
	WinInputController(const WinDeviceDesc& desc);
	virtual ~WinInputController();

	virtual InputChangeBuffer UpdateSettings() = 0;

protected:
	uint32 axes;
	uint32 buttons;
	int fd;
};

} /* namespace InputService */

#endif /* PLUGINS_INPUTSERVICE_LOCAL_INCLUDE_PLATFORM_WIN32_WININPUTCONTROLLER_H_ */
