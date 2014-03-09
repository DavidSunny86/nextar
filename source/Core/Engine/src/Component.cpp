/*
 * Component.cpp
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */
#include "NexEngine.h"
#include "Component.h"
#include "ComponentFactoryArchive.h"

namespace nextar {

	/*********************************
	 * Component
	 *********************************/
	Component::Component(const StringID compName, Component* parentObject) :
		NamedObject(compName),
		flags(ENABLED), parent(parentObject) {
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

	Component* Component::FindComponent(const StringID name) {
		return 0;
	}

	/*********************************
	 * SharedComponent
	 *********************************/
	SharedComponentPtr SharedComponent::Null;
	SharedComponent::SharedComponent(const StringID name,
			Component* parent, Group* addToGroup) :
		group(nullptr) {
		SetID(name);
		SetParent(parent);
		if (addToGroup) {
			AddToGroup(addToGroup);
		}
	}

	SharedComponent::~SharedComponent() {
		NEX_ASSERT(group==nullptr);
	}

	void SharedComponent::AddToGroup(Group* g) {
		if (group) {
			group->AsyncRemove(GetID());
		}
		group = g;
		NEX_ASSERT(group);
		SharedComponentPtr thisPtr = Bind(this);
		group->AsyncAdd(thisPtr);
	}

	void SharedComponent::RemoveFromGroup() {
		if (group) {
			group->AsyncRemove(GetID());
			group = nullptr;
		}
	}

	/*********************************
	 * Component::Factory
	 *********************************/
	Component::Factory::Factory(const StringID name) : NamedObject(name) {
	}

	Component::Factory::~Factory() {
	}

	/*********************************
	 * SharedComponent::Group
	 *********************************/
	SharedComponent::Group::Group(const StringID name) : NamedObject(name) {
	}

	SharedComponent::Group::~Group() {
	}

} /* namespace nextar */

