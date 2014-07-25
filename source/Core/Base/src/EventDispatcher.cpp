/*
 * EventDispatcher.cpp
 *
 *  Created on: 15-Jul-2014
 *      Author: obhi
 */

#include <EventDispatcher.h>

namespace nextar {

EventDispatcher::EventDispatcher() {
}

EventDispatcher::~EventDispatcher() {
}

void EventDispatcher::Subscribe(EventID event, EventCallback callback, void* data) {
	subscribers.insert(KeyValuePair(event, ValueType(callback, data)));
}

void EventDispatcher::Unsubscribe(EventID event, EventCallback callback) {
	auto items = subscribers.equal_range(event);
	for(auto it = items.first; it != items.second; ) {
		if ((*it).second.first == callback) {
			it = subscribers.erase(it);
		} else
			++it;
	}
}

void EventDispatcher::DispatchEvent(EventID event) {
	auto items = subscribers.equal_range(event);
	for(auto it = items.first; it != items.second; ) {
		(*(*it).second.first)((*it).second.second);
	}
}

} /* namespace nextar */
