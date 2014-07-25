/*
 * EventDispatcher.h
 *
 *  Created on: 15-Jul-2014
 *      Author: obhi
 */

#ifndef EVENTDISPATCHER_H_
#define EVENTDISPATCHER_H_

#include <NexBase.h>
#include <CommonPools.h>

namespace nextar {

/*
 *
 */
class _NexBaseAPI EventDispatcher {
public:

	typedef void (*EventCallback) (void* param);
	EventDispatcher();

	void Subscribe(EventID event, EventCallback callback, void* param);
	void Unsubscribe(EventID event, EventCallback callback);
	void DispatchEvent(EventID event);

protected:
	~EventDispatcher();

	typedef std::pair<EventCallback, void*> ValueType;
	typedef std::pair<EventID, ValueType> KeyValuePair;
	typedef multimap<EventID, ValueType, std::less<EventID>, STDPoolAllocator<KeyValuePair, 16, MEMCAT_GENERAL> >::type SubscriberList;

	SubscriberList subscribers;
};

} /* namespace nextar */

#endif /* EVENTDISPATCHER_H_ */
