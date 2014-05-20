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
	ComponentGroupSet::ComponentGroupSet(const StringID name) : SharedComponent::Group(name) {
	}

	ComponentGroupSet::~ComponentGroupSet() {
		// make sure clean up was called
		//NEX_ASSERT(componentMap.size() == 0);
	}

	void ComponentGroupSet::AsyncAdd(SharedComponentPtr& c) {
		/** Delete all components make sure they are destructible */
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		Debug(
				GetName() + ": Number of components = "
						+ Convert::ToString((uint32) componentMap.size()));
		componentMap.insert(ComponentMap::value_type(c->GetID(), c));
	}

	SharedComponentPtr& ComponentGroupSet::AsyncFind(const StringID name) {
		/* No sub component by default */
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		ComponentMap::iterator it = componentMap.find(name);
		if (it != componentMap.end()) {
			return (*it).second;
		}
		return SharedComponent::Null;
	}

	void ComponentGroupSet::AsyncRemove(const StringID name) {
		/* No sub component by default */
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		componentMap.erase(name);
	}

	void ComponentGroupSet::AsyncRemoveAll(uint32 ofType) {
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		if (ofType == Component::CLASS_UNKNOWN)
			componentMap.clear();
		else {
			auto a = componentMap.begin();
			while(a != componentMap.end()) {
				if ((*a).second->GetClassID() == ofType)
					a = componentMap.erase(a);
			}
		}
	}

	void ComponentGroupSet::AsyncCollect(SharedComponent::Group* container, 
		uint32 ofType) {
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		NEX_ASSERT (ofType != Component::CLASS_UNKNOWN);
		auto a = componentMap.begin();
		while(a != componentMap.end()) {
			if ((*a).second->GetClassID() == ofType)
				container->AsyncAdd((*a).second);
		}
	}
} /* namespace nextar */
