/*
 * XBox360Controller.cpp
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#include <XBox360Controller.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

namespace InputService {

XBox360Controller::XBox360Controller(const UxDeviceDesc& desc) :
	UxInputController(desc) {
}

XBox360Controller::~XBox360Controller() {
}

InputChangeBuffer XBox360Controller::UpdateSettings() {
	InputChangeBuffer buffer;
	return buffer;
}

bool XBox360Controller::IsDown(KeyID keyId) {
	return false;
}

bool XBox360Controller::IsOn(KeyID keyId) {
	return false;
}

void XBox360Controller::PollData() {
}

int32 XBox360Controller::GetValue(KeyID keyId) {
	return 0;
}

InputDir XBox360Controller::GetDir(KeyID keyId) {
	return InputDir();
}

} /* namespace InputService */
