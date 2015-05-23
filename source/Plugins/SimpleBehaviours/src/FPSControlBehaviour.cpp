/*
 * FPSControlBehaviour.cpp
 *
 *  Created on: 24-May-2015
 *      Author: obhi
 */

#include <InputManager.h>
#include <FPSControlBehaviour.h>

namespace SimpleBehaviours {

FPSControlBehaviour::FPSControlBehaviour(uint16 deviceId) {
	InputManager::Instance().RegisterListener(this, deviceId);
}

FPSControlBehaviour::~FPSControlBehaviour() {
}



} /* namespace SimpleBehaviours */
