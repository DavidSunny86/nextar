/*
 * ComponentGroupArchive.cpp
 *
 *  Created on: 01-Mar-2014
 *      Author: obhi
 */

#include <NexEngine.h>
#include <ComponentGroupArchive.h>
#include <ComponentGroupSet.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(ComponentGroupArchive);

ComponentGroupArchive ::ComponentGroupArchive() :
		defaultGroup(nullptr) {
	// TODO Auto-generated constructor stub

}

ComponentGroupArchive::~ComponentGroupArchive() {
	AsyncDeleteAll();
}

void ComponentGroupArchive::AsyncAddGroup(SharedComponent::Group* group) {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	if (group->GetID() == StringUtils::DefaultID)
		defaultGroup = group;
	componentGroups[group->GetID()] = group;
}

void ComponentGroupArchive::AsyncRemoveGroup(StringID name, bool d) {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	auto it = componentGroups.find(name);
	if (it != componentGroups.end()) {
		if (d)
			NEX_DELETE((*it).second);
		componentGroups.erase(it);
	}
	if (name == StringUtils::DefaultID)
		defaultGroup = nullptr;
}

SharedComponent::Group* ComponentGroupArchive::_AsyncFindGroup(StringID name) {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	ComponentGroupMap::iterator it = componentGroups.find(name);
	if (it != componentGroups.end())
		return (*it).second;
	return 0;
}

SharedComponent::Group* ComponentGroupArchive::_AsyncFindOrCreate(
		StringID name) {
	SharedComponent::Group* f = _AsyncFindGroup(name);
	if (f)
		return f;
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	f = NEX_NEW(ComponentGroupSet(name));
	componentGroups.emplace(name, f);
	return f;
}

void ComponentGroupArchive::AsyncDeleteAll() {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	for (auto &v : componentGroups)
		if (v.second)
			NEX_DELETE(v.second);
	componentGroups.clear();
}

void ComponentGroupArchive::Configure(const Config& config) {
}

} /* namespace nextar */
