/*
 * Component.cpp
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "Component.h"
#include "ComponentFactoryArchive.h"

namespace nextar {

	/*********************************
	 * Component
	 *********************************/
	Component::Component(const String& compName, Component* parentObject) :
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

	Component* Component::FindComponent(const String& name) {
		StringPair namePair = StringUtils::Split(name, '.');
		Component* c = FindComponent(nameTable.AsyncStringID(namePair.first));
		if (!namePair.second.length())
			return c;
		else
			return c->FindComponent(namePair.second);
		return nullptr;
	}

	/*********************************
	 * SharedComponent
	 *********************************/
	SharedComponentPtr SharedComponent::Null;
	SharedComponent::SharedComponent(const String& name,
			Component* parent, Group* addToGroup) :
		group(nullptr) {
		SetName(name);
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
	Component::Factory::Factory(const String& name) : NamedObject(name) {
	}

	Component::Factory::~Factory() {
	}

	/*********************************
	 * SharedComponent::Group
	 *********************************/
	SharedComponent::Group::Group(const String& name) : NamedObject(name) {
	}

	SharedComponent::Group::~Group() {
	}

	Component* SharedComponent::Group::Find(const String& name) {
		if (name.length()) {
			StringPair namePair = StringUtils::Split(name, '.');
			SharedComponentPtr& ptr = AsyncFind(nameTable.AsyncStringID(namePair.first));
			if (!namePair.second.length())
				return ptr;
			else
				return ptr->FindComponent(namePair.second);
		}
		return nullptr;
	}
} /* namespace nextar */

