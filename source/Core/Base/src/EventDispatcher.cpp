/*
 * EventDispatcher.cpp
 *
 *  Created on: 15-Jul-2014
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <EventDispatcher.h>

namespace nextar {

EventDispatcher::EventDispatcher() : isDispatching(false) {
}

EventDispatcher::~EventDispatcher() {
}

void EventDispatcher::Subscribe(EventID event, EventCallback callback, void* data) {
	if (isDispatching) {
		toAdd.push_back(KeyValuePair(event, ValueType(callback, data)));
	} else {
		subscribers.insert(KeyValuePair(event, ValueType(callback, data)));
	}
}

void EventDispatcher::Unsubscribe(EventID event, EventCallback callback) {
	if (isDispatching) {
		toRemove.push_back(KeyValuePair(event, ValueType(callback, nullptr)));
	} else {
		auto items = subscribers.equal_range(event);
		for(auto it = items.first; it != items.second; ) {
			if ((*it).second.first == callback) {
				it = subscribers.erase(it);
			} else
				++it;
		}
	}
}

void EventDispatcher::DispatchEvent(EventID event) {
	isDispatching = true;
	auto items = subscribers.equal_range(event);
	for(auto it = items.first; it != items.second; ++it) {
		(*(*it).second.first)((*it).second.second);
	}
	isDispatching = false;
	if (toAdd.size() > 0) {
		for(auto& e : toAdd) {
			Subscribe(e.first, e.second.first, e.second.second);
		}
	}

	if (toRemove.size() > 0) {
		for(auto& e : toAdd) {
			Unsubscribe(e.first, e.second.first);
		}
	}
}

} /* namespace nextar */
