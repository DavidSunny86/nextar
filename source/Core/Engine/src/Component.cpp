/*
 * Component.cpp
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */
#include <NexEngine.h>
#include <Component.h>
#include <ComponentFactoryArchive.h>

namespace nextar {

/*********************************
 * Component
 *********************************/
Component::Component(const StringID compName, Component* parentObject) :
		NamedObject(compName), flags(ENABLED), parent(parentObject) {
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

Component* Component::Instance(uint32 classId, StringID name,
			Component::Factory* factory) {
	if (!factory)
		factory = ComponentFactoryArchive::Instance().AsyncFindFactory(classId);
	if (factory) {
		return factory->AsyncCreate(classId, name);
	}
	return nullptr;
}

/*********************************
 * SharedComponent
 *********************************/
SharedComponentPtr SharedComponent::Null;
SharedComponent::SharedComponent(const StringID name, Component* parent,
		Group* addToGroup) :
		group(nullptr) {
	SetID(name);
	SetParent(parent);
	if (addToGroup) {
		AddToGroup(addToGroup);
	}
}

SharedComponent::~SharedComponent() {
	NEX_ASSERT(group == nullptr);
}

SharedComponent::InstanceResult SharedComponent::Instance(SharedComponentPtr& oInst, uint32 classId, StringID name,
		StringID factory, Group* group) {
	Group::Lock groupLock(group);
	if (group) {
		oInst = group->Find(name);
		if (oInst)
			return SharedComponent::INSTANCE_RETRIEVED;
	}

	Factory* factoryPtr =
						ComponentFactoryArchive::Instance().AsyncFindFactory(
								classId, factory);
	Component* component = Component::Instance(classId, name, factoryPtr);
	if (component) {
		oInst = Assign<SharedComponent>(component);
		if (group)
			group->Add(oInst);
		return SharedComponent::INSTANCE_CREATED;
	}
	return SharedComponent::INSTANCE_FAILED;
}

SharedComponent::InstanceResult SharedComponent::Instance(SharedComponentPtr& oInst, uint32 classId, StringID name,
		Factory* factory, Group* group) {
	Group::Lock groupLock(group);
	if (group) {
		oInst = group->Find(name);
		if (oInst)
			return SharedComponent::INSTANCE_RETRIEVED;
	}
	Component* component = Component::Instance(classId, name, factory);
	if (component) {
		oInst = Assign<SharedComponent>(component);
		if (group)
			group->Add(oInst);
		return SharedComponent::INSTANCE_CREATED;
	}
	return SharedComponent::INSTANCE_FAILED;
}

void SharedComponent::AddToGroup(Group* g) {
	if (group) {
		Group::Lock groupLock(group);
		group->Remove(GetID());
	}
	group = g;
	NEX_ASSERT(group);
	SharedComponentPtr thisPtr = Bind(this);
	if (group) {
		Group::Lock groupLock(group);
		group->Add(thisPtr);
	}
}

void SharedComponent::RemoveFromGroup() {
	if (group) {
		Group::Lock groupLock(group);
		group->Remove(GetID());
		group = nullptr;
	}
}

/*********************************
 * Component::Factory
 *********************************/
Component::Factory::Factory(const StringID name) :
		NamedObject(name) {
}

Component::Factory::~Factory() {
}

/*********************************
 * SharedComponent::Group
 *********************************/
SharedComponent::Group::Group(const StringID name) :
		NamedObject(name) {
}

SharedComponent::Group::~Group() {
}

} /* namespace nextar */

