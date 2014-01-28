/*
 * Component.cpp
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "Component.h"
#include "ComponentManager.h"
#include "ComponentManagerArchive.h"
//#include "ComponentStreamer.h"

namespace nextar {
		

	Component::Component(ComponentManager* componentCreator, const String& compName, Component* parentObject) :
		creator(componentCreator), 
		flags(ENABLED), parent(parentObject) {
		SetName(compName);
		NEX_ASSERT(creator);
	}

	Component::~Component() {
	}

	void Component::SetEnabled(bool enabled) {
		if (enabled)
			flags |= ENABLED;
		else
			flags &= ~ENABLED;
	}

	void Component::SetParent(Component* c) {
		parent = c;
	}

	Component* Component::AsyncFindChild(const StringID name) {
		return 0;
	}

} /* namespace nextar */

