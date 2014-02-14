/*
 * FactoryRoot.cpp
 *
 *  Created on: Oct 6, 2011
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "ComponentFactoryArchive.h"

namespace nextar {

	NEX_DEFINE_SINGLETON_PTR(ComponentFactoryArchive);
	ComponentFactoryArchive::ComponentFactoryArchive() {
	}

	ComponentFactoryArchive::~ComponentFactoryArchive() {
		AsyncDeleteAll();
	}

	void ComponentFactoryArchive::AsyncAddFactory(uint32 type, Component::Factory* factory) {
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		String name = factory->GetName() + "::" + Convert::ToString((int32)type, ' ', std::ios::hex);
		componentFactories[name] = factory;
	}

	void ComponentFactoryArchive::AsyncRemoveFactory(uint32 type, const String& name, bool d) {

		if (type == Component::COMPONENT_UNKNOWN) {
			String propName = name + "::";
			NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
			for( auto p = componentFactories.cbegin(); p != componentFactories.cend(); ) {
				if( (*p).first.compare(0, propName.length(), propName) == 0 ) {
					if (d)
						NEX_DELETE (*p).second;
					componentFactories.erase(p++);
				}
				else {
					++p;
				}
			}
		} else {
			String mname = name + "-" + Convert::ToString((int32)type, ' ', std::ios::hex);
			NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
			auto it = componentFactories.find(mname);
			if (it != componentFactories.end()) {
				if (d)
					NEX_DELETE (*it).second;
				componentFactories.erase(it);
			}
		}
	}

	Component::Factory* ComponentFactoryArchive::AsyncFindFactory(uint32 type, const String& name) {
		String mname = name + "::" + Convert::ToString((int32)type, ' ', std::ios::hex);
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		ComponentFactoryMap::iterator it = componentFactories.find(mname);
		if ( it != componentFactories.end() )
			return (*it).second;
		return 0;
	}

	void ComponentFactoryArchive::AsyncDeleteAll() {
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		for(auto &v : componentFactories)
			if ( v.second )
				NEX_DELETE v.second;
		componentFactories.clear();
	}

	void ComponentFactoryArchive::Configure(const Config& config) {
		for(auto v : componentFactories)
			if ( v.second )
				v.second->Configure(config);
	}

} /* namespace nextar */
