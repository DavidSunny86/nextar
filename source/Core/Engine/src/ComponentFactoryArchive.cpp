/*
 * FactoryRoot.cpp
 *
 *  Created on: Oct 6, 2011
 *      Author: obhi
 */
#include <NexEngine.h>
#include <ComponentFactoryArchive.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(ComponentFactoryArchive);ComponentFactoryArchive ::ComponentFactoryArchive() {
}

ComponentFactoryArchive::~ComponentFactoryArchive() {
	AsyncDeleteAll();
}

void ComponentFactoryArchive::AsyncAddFactory(uint32 type,
		Component::Factory* factory) {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	componentFactories[FactoryID(factory->GetID(), type)] = factory;
}

void ComponentFactoryArchive::AsyncRemoveFactory(uint32 type, StringID name,
		bool d) {
	if (type == Component::CLASS_UNKNOWN) {
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		for (auto p = componentFactories.cbegin();
				p != componentFactories.cend();) {
			if ((*p).first.first == name) {
				if (d)
					NEX_DELETE((*p).second);
				componentFactories.erase(p++);
			} else {
				++p;
			}
		}
	} else {
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		auto it = componentFactories.find(FactoryID(name, type));
		if (it != componentFactories.end()) {
			if (d)
				NEX_DELETE((*it).second);
			componentFactories.erase(it);
		}
	}
}

Component::Factory* ComponentFactoryArchive::AsyncFindFactory(uint32 type,
		const StringID name) {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	ComponentFactoryMap::iterator it = componentFactories.find(
			FactoryID(name, type));
	if (it != componentFactories.end())
		return (*it).second;
	return 0;
}

void ComponentFactoryArchive::AsyncDeleteAll() {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	for (auto &v : componentFactories)
		if (v.second)
			NEX_DELETE(v.second);
	componentFactories.clear();
}

void ComponentFactoryArchive::Configure(const Config& config) {
	for (auto v : componentFactories)
		if (v.second)
			v.second->Configure(config);
}

} /* namespace nextar */
