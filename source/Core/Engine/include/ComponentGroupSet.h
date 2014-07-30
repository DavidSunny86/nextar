/*
 * ComponentManager.h
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */

#ifndef COMPONENTMANAGER_H_
#define COMPONENTMANAGER_H_

#include <NexBase.h>
#include <Component.h>
#include <NexThread.h>

namespace nextar {

class Component;
class ComponentFactoryArchive;
class ComponentStreamer;

class _NexEngineAPI ComponentGroupSet: public SharedComponent::Group {
	NEX_LOG_HELPER(ComponentGroupSet)
	;

public:

	ComponentGroupSet(const StringID name);
	virtual ~ComponentGroupSet();

	virtual void AcquireLock();
	virtual void ReleaseLock();
	virtual void Add(SharedComponentPtr&) override;
	virtual SharedComponentPtr& Find(uint32 classId, const StringID name) override;
	virtual void Remove(uint32 classId, StringID name) override;
	virtual void Remove(StringID name) override;

	virtual void AsyncRemoveAll(uint32 ofType = Component::CLASS_UNKNOWN)
			override;
	virtual void AsyncCollect(SharedComponent::Group* container, uint32 ofType)
			override;

protected:

	NEX_THREAD_MUTEX(containerLock);
	// todo Might choose to support same named objects but of different types
	typedef multimap<StringID, SharedComponentPtr>::type ComponentMap;
	ComponentMap componentMap;
};

}
/* namespace nextar */
#endif /* COMPONENTMANAGER_H_ */
