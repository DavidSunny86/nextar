/*
 * ComponentArchive.cpp
 *
 *  Created on: 17-Feb-2014
 *      Author: obhi
 */

#include <ComponentArchive.h>

namespace nextar {

	NEX_DEFINE_SINGLETON_PTR(ComponentArchive);

	ComponentArchive::ComponentArchive() {
	}

	ComponentArchive::~ComponentArchive() {
	}

	void ComponentArchive::AsyncAdd(SharedComponent::Group* group) {
		NEX_THREAD_LOCK_GUARD_MUTEX(archiveLock);
		groups.insert(GroupPair(group->GetID(), group));
	}

	void ComponentArchive::AsyncRemove(StringID name, bool deleteGroup) {
		NEX_THREAD_LOCK_GUARD_MUTEX(archiveLock);
		GroupMap::iterator it = groups.find(name);
		if (deleteGroup)
			NEX_DELETE((*it).second);
		groups.erase(it);
	}

	void ComponentArchive::AsyncRemoveAll() {
		NEX_THREAD_LOCK_GUARD_MUTEX(archiveLock);
		for(auto &it : groups) {
			NEX_DELETE(it.second);
		}
		groups.clear();
	}

	SharedComponent::Group* ComponentArchive::AsyncFind(StringID name) {
		NEX_THREAD_LOCK_GUARD_MUTEX(archiveLock);
		GroupMap::iterator it = groups.find(name);
		if (it != groups.end())
			return (*it).second;
		return nullptr;
	}
} /* namespace nextar */
