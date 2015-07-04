/*
 * Behavior.cpp
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#include <Behaviour.h>

namespace nextar {

Behaviour::Behaviour(const StringID name, const StringID factory,
		Component* parent) : Component(name, factory, parent) {
}

} /* namespace nextar */
