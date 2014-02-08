/*
 * Renderable.cpp
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#include "Renderable.h"

namespace nextar {

	Renderable::Renderable(ComponentManager* creator, const String& name, Component* parent) :
		Spatial(creator, name, parent) {
	}

	Renderable::~Renderable() {
	}

} /* namespace nextar */
