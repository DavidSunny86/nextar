/*
 * ManagerRoot.cpp
 *
 *  Created on: Oct 6, 2011
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "ComponentManagerArchive.h"

namespace nextar {

	NEX_DEFINE_SINGLETON_PTR(ComponentManagerArchive);
	ComponentManagerArchive::ComponentManagerArchive() {
	}

	ComponentManagerArchive::~ComponentManagerArchive() {
		AsyncDeleteAll();
	}

	void ComponentManagerArchive::AsyncAddManager(int type, ComponentManager* manager) {
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		String name = manager->GetName() + "::" + Convert::ToString((int32)type, ' ', std::ios::hex);
		componentManagers[name] = manager;
	}

	void ComponentManagerArchive::AsyncRemoveManager(int type, const String& name, bool d) {

		if (type == Component::COMPONENT_UNKNOWN) {
			String propName = name + "::";
			NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
			for( auto p = componentManagers.cbegin(); p != componentManagers.cend(); ) {
				if( (*p).first.compare(0, propName.length(), propName) == 0 ) {
					if (d)
						NEX_DELETE (*p).second;
					componentManagers.erase(p++);
				}
				else {
					++p;
				}
			}
		} else {
			String mname = name + "-" + Convert::ToString((int32)type, ' ', std::ios::hex);
			NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
			auto it = componentManagers.find(mname);
			if (it != componentManagers.end()) {
				if (d)
					NEX_DELETE (*it).second;
				componentManagers.erase(it);
			}
		}
	}

	ComponentManager* ComponentManagerArchive::AsyncFindManager(int type, const String& name) {
		String mname = name + "::" + Convert::ToString((int32)type, ' ', std::ios::hex);
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		ComponentManagerMap::iterator it = componentManagers.find(mname);
		if ( it != componentManagers.end() )
			return (*it).second;
		return 0;
	}

	void ComponentManagerArchive::AsyncDeleteAll() {
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		for(auto v : componentManagers)
			if ( v.second )
				NEX_DELETE v.second;
		componentManagers.clear();
	}

	void ComponentManagerArchive::Configure(const Config& config) {
		for(auto v : componentManagers)
			if ( v.second )
				v.second->Configure(config);
	}

} /* namespace nextar */
