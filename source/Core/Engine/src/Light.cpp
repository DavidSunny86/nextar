/*
 * Light.cpp
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#include "NexHeaders.h"
#include "Light.h"
#include "LightSystem.h"

namespace nextar {

	Light::Light(const String& name) : Renderable(name), sortKey(0) {
	}

	Light::~Light() {
	}

	uint32 Light::GetClassID() const {
		return CLASS_ID;
	}

	void Light::Visit(SceneTraversal& traversal) {
		traversal.lightSystem->PushLight(sortKey, this);
	}

} /* namespace nextar */
