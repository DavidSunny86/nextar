/*
 * ComponentArchive.h
 *
 *  Created on: 17-Feb-2014
 *      Author: obhi
 */

#ifndef COMPONENTARCHIVE_H_
#define COMPONENTARCHIVE_H_

#include <Component.h>

namespace nextar {

class ComponentArchive: public Singleton<ComponentArchive>,
		public AllocComponent {
public:
	typedef nextar::SharedComponent::Group Group;
	typedef map<StringID, Group*>::type GroupMap;
	typedef std::pair<StringID, Group*> GroupPair;

	ComponentArchive();
	virtual ~ComponentArchive();

	void AsyncAdd(Group*);
	Group* AsyncFind(const StringID name);
	void AsyncRemove(StringID name, bool deleteGroup = true);
	void AsyncRemoveAll();

protected:
	NEX_THREAD_MUTEX(archiveLock);
	GroupMap groups;
};

}
/* namespace nextar */

#endif /* COMPONENTARCHIVE_H_ */
