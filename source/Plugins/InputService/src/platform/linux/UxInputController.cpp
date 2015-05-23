/*
 * UxInputController.cpp
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#include <UxInputController.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

namespace InputService {

const UxDeviceDesc UxDeviceDesc::Null;

UxInputController::UxInputController(const UxDeviceDesc& desc) :
	InputController(desc.info), fd(desc.fd),
	axes(desc.axes), buttons(desc.buttons) {
}

UxInputController::~UxInputController() {
	if (fd >= 0)
		close(fd);
}

} /* namespace InputService */
