/*
 * Renderable.cpp
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#include <Renderable.h>

namespace nextar {

Renderable::Renderable(const StringID name, const StringID factory, Component* parent) :
		Spatial(name, factory, parent) {
}

Renderable::~Renderable() {
}

} /* namespace nextar */
