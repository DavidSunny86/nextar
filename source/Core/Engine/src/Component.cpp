/*
 * Component.cpp
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */
#include <NexEngine.h>
#include <Component.h>
#include <ComponentFactoryArchive.h>
#include <Entity.h>

namespace nextar {

/*********************************
 * Component
 *********************************/
Component::ID Component::NullID(StringUtils::NullID, StringUtils::NullID);
Component::Component(const StringID compName, const StringID creator, Component* parentObject) :
		NamedObject(compName), factory(creator), flags(ENABLED), parent(parentObject) {
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

Entity* Component::GetEntity() {
	Component* p = this;
	while (p && p->GetClassID() != Entity::CLASS_ID) {
		p = p->GetParent();
	}
	return static_cast<Entity*>(p);
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

Component::ID Component::ParseID(const String& name) {
	Component::ID ret;
	auto factoryNamePair = StringUtils::Split(name, ':');
	if (factoryNamePair.first.length() && factoryNamePair.second.length()) {
		ret.factory = NamedObject::AsyncStringID(factoryNamePair.first);
	} else {
		factoryNamePair.second = std::move(factoryNamePair.first);
	}
	if (factoryNamePair.second.length()) {
		ret.name = NamedObject::AsyncStringID(factoryNamePair.second);
	}

	return ret;
}
/*********************************
 * SharedComponent
 *********************************/
SharedComponent::ID SharedComponent::NullID(StringUtils::NullID, StringUtils::NullID, StringUtils::NullID);
SharedComponentPtr SharedComponent::Null;

SharedComponent::SharedComponent(const StringID name,
		const StringID factory,
		Component* parent,
		Group* addToGroup) :
		group(nullptr) {
	SetID(name);
	SetFactory(factory);
	SetParent(parent);

	if (addToGroup) {
		AddToGroup(addToGroup);
	}
}

SharedComponent::~SharedComponent() {
	//NEX_ASSERT(group == nullptr);
}

SharedComponent::InstanceResult SharedComponent::Instance(SharedComponentPtr& oInst, uint32 classId, StringID name,
		StringID factory, Group* group) {
	Group::Lock groupLock(group);
	if (group) {
		oInst = group->Find(classId, name);
		if (oInst)
			return SharedComponent::INSTANCE_RETRIEVED;
	}

	Factory* factoryPtr =
						ComponentFactoryArchive::Instance().AsyncFindFactory(
								classId, factory);
	Component* component = Component::Instance(classId, name, factoryPtr);
	if (component) {
		oInst = Assign<SharedComponent>(component);
		if (group) {
			group->Add(oInst);
			oInst->group = group;
		}
		return SharedComponent::INSTANCE_CREATED;
	}
	return SharedComponent::INSTANCE_FAILED;
}

SharedComponent::InstanceResult SharedComponent::Instance(SharedComponentPtr& oInst, uint32 classId, StringID name,
		Factory* factory, Group* group) {
	Group::Lock groupLock(group);
	if (group) {
		oInst = group->Find(classId, name);
		if (oInst)
			return SharedComponent::INSTANCE_RETRIEVED;
	}
	Component* component = Component::Instance(classId, name, factory);
	if (component) {
		oInst = Assign<SharedComponent>(component);
		if (group) {
			group->Add(oInst);
			oInst->group = group;
		}
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

SharedComponent::ID SharedComponent::ToID(const String& name) {
	SharedComponent::ID ret;
	auto factoryNamePair = StringUtils::Split(name, ':');
	if (factoryNamePair.second.length()) {
		ret.factory = NamedObject::AsyncStringID(factoryNamePair.first);
	} else {
		factoryNamePair.second = std::move(factoryNamePair.first);
	}
	if (factoryNamePair.second.length()) {
		auto groupNamePair = StringUtils::Split(factoryNamePair.second, '.');
		if (groupNamePair.second.length())
			ret.group = NamedObject::AsyncStringID(groupNamePair.first);
		else
			groupNamePair.second = std::move(groupNamePair.first);
		ret.name = NamedObject::AsyncStringID(groupNamePair.second);
	}

	return ret;
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

