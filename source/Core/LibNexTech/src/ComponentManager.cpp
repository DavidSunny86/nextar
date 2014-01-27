/*
 * ComponentManagerImpl.cpp
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "ComponentManager.h"
#include "ComponentManagerArchive.h"
#include "ApplicationContext.h"

namespace nextar {

	/********************************************************************
	 * ComponentManagerImpl
	 ********************************************************************/
	ComponentManagerImpl::ComponentManagerImpl(const String& name) : ComponentManager(name) {

	}

	ComponentManagerImpl::~ComponentManagerImpl() {
		// make sure clean up was called
		NEX_ASSERT(componentMap.size() == 0);
	}

	void ComponentManagerImpl::AsyncCleanup() {
		/** Delete all components make sure they are destructible */
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		Trace(
				GetName() + ": Number of unused components = "
						+ Convert::ToString((uint32) componentMap.size()));
		ComponentMap::iterator en = componentMap.end();
		for (ComponentMap::iterator it = componentMap.begin(); it != en; ++it) {
			AsyncDestroyImpl((*it).second);
		}
		componentMap.clear();
	}

	Component* ComponentManagerImpl::AsyncCreate(int type, const String& name) {
		Component* c = AsyncCreateImpl(type, name);
		if (c) {
			NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
			componentMap.insert(ComponentMap::value_type(name, c));
		}
		return c;
	}

	void ComponentManagerImpl::AsyncDestroy(Component* c) {
		{
			NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
			ComponentMap::iterator it = componentMap.find(c->GetName());
			if (it != componentMap.end()) {
				componentMap.erase(it);
			}
		}
		AsyncDestroyImpl(c);
	}

	Component* ComponentManagerImpl::AsyncFind(const String& name) {
		/* No sub component by default */
		if (name.length()) {
			size_t pos = name.find_first_of(':');
			const String* namePtr = &name;
			String tmp;
			if (pos != String::npos) {
				tmp = name.substr(0, pos);
				namePtr = &tmp;
			}
			{
				NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
				ComponentMap::iterator it = componentMap.find(*namePtr);
				if (it != componentMap.end()) {
					if (pos == String::npos)
						return (*it).second;
					else
						return (*it).second->AsyncFindChild(
								name.substr(pos + 1));
				}
			}
		}
		return 0;
	}

	Component* ComponentManagerImpl::AsyncFindOrCreate(int type, const String& name) {
		Component* c = AsyncFind(name);
		if (!c) {
			c = AsyncCreate(type, name);
		}
		return c;
	}

	void ComponentManagerImpl::AsyncDestroyImpl(Component* component) {
		NEX_DELETE component;
	}

	/********************************************************************
	 * ComponentManager
	 ********************************************************************/
	ComponentManager::ComponentManager(const String& _name) : name(_name) {
	}

	ComponentManager::~ComponentManager() {
	}

	Component* ComponentManager::AsyncFindComponent(int type, const String& comp) {
		StringUtils::StringPair name = StringUtils::Split(comp);
		ComponentManager* found = ComponentManagerArchive::Instance().AsyncFindManager(type, name.second);
		if (found) {
			return found->AsyncFind(name.first);
		}

		return 0;
	}

	Component* ComponentManager::AsyncFindOrCreateComponent(int type, const String& comp) {
		StringUtils::StringPair name = StringUtils::Split(comp);
		ComponentManager* found = ComponentManagerArchive::Instance().AsyncFindManager(type, name.second);
		if (found) {
			return found->AsyncFindOrCreate(type, name.first);
		}
		return 0;
	}

	void ComponentManager::Configure(const Config& config) {
	}
} /* namespace nextar */
