/*
 * ComponentManagerImpl.cpp
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */
#include <NexEngine.h>
#include <ComponentGroupSet.h>
#include <ComponentFactoryArchive.h>
#include <ApplicationContext.h>

namespace nextar {

/********************************************************************
 nbvc	 * ComponentManagerImpl
 ********************************************************************/
ComponentGroupSet::ComponentGroupSet(const StringID name) :
		SharedComponent::Group(name) {
}

ComponentGroupSet::~ComponentGroupSet() {
	// make sure clean up was called
	//NEX_ASSERT(componentMap.size() == 0);
}

void ComponentGroupSet::AcquireLock() {
	NEX_THREAD_MUTEX_LOCK(containerLock);
}

void ComponentGroupSet::ReleaseLock() {
	NEX_THREAD_MUTEX_UNLOCK(containerLock);
}

void ComponentGroupSet::Add(SharedComponentPtr& c) {
	/** Delete all components make sure they are destructible */
	Debug(
			GetName() + ": Number of components = "
					+ Convert::ToString((uint32) componentMap.size()));
	StringID name = c->GetID();
	componentMap.insert(ComponentMap::value_type(name, c));
}

SharedComponentPtr& ComponentGroupSet::Find(uint32 classId, const StringID name) {
	/* No sub component by default */
	auto range = componentMap.equal_range(name);
	if (range.first != componentMap.end()) {
		for (auto it = range.first; it != range.second; ++it)
			if ( (*it).second->GetClassID() == classId )
				return (*it).second;
	}
	return SharedComponent::Null;
}

void ComponentGroupSet::Remove(uint32 classId, const StringID name) {
	/* No sub component by default */
	auto range = componentMap.equal_range(name);
	if (range.first != componentMap.end()) {
		for (auto it = range.first; it != range.second; ++it)
			if ( (*it).second->GetClassID() == classId ) {
				componentMap.erase(it);
				return;
			}
	}
}

void ComponentGroupSet::Remove(const StringID name) {
	/* No sub component by default */
	componentMap.erase(name);
}

void ComponentGroupSet::AsyncRemoveAll(uint32 ofType) {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	if (ofType == Component::CLASS_UNKNOWN)
		componentMap.clear();
	else {
		auto a = componentMap.begin();
		while (a != componentMap.end()) {
			if ((*a).second->GetClassID() == ofType)
				a = componentMap.erase(a);
		}
	}
}

void ComponentGroupSet::AsyncCollect(SharedComponent::Group* container,
		uint32 ofType) {
	Group::Lock otherLock(container);
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	NEX_ASSERT(ofType != Component::CLASS_UNKNOWN);
	auto a = componentMap.begin();
	while (a != componentMap.end()) {
		if ((*a).second->GetClassID() == ofType)
			container->Add((*a).second);
	}
}

} /* namespace nextar */
