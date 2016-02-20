/*
 * BlockCommandHandler.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#include <BlockCommandHandler.h>

namespace nextar {

BlockCommandHandler::BlockCommandHandler() : CommandHandler(true) {

}

BlockCommandHandler::~BlockCommandHandler() {
	//for(auto& e : _handlers) {
		//NEX_DELETE(e.second);
	//}
}

const CommandHandler* BlockCommandHandler::GetHandler(
		const String& name) const {
	auto it = _handlers.find(name);
	if (it != _handlers.end()) {
		return (*it).second;
	}
	return nullptr;
}

void BlockCommandHandler::AddHandler(const String& qualifiedName,
		CommandHandler* handler) {
	StringPair p = StringUtils::Split(qualifiedName, '.');
	if (p.second.length() > 0) {
		CommandHandler* handler = GetHandler(p.first);
		if (handler->IsBlockHandler()) {
			static_cast<BlockCommandHandler*>(handler)->AddHandler(p.second, handler);
		}
	} else {
		auto& e = _handlers[p.first];
		if (e)
			NEX_DELETE(e);
		e = handler;
		e->SetParentBlock(this);
	}
}

void BlockCommandHandler::RemoveHandler(const String& qualifiedName) {
	StringPair p = StringUtils::Split(qualifiedName, '.');
	if (p.second.length() > 0) {
		CommandHandler* handler = GetHandler(p.first);
		if (handler->IsBlockHandler()) {
			static_cast<BlockCommandHandler*>(handler)->RemoveHandler(p.second);
		}
	} else {
		_handlers.erase(p.first);
		//auto it = _handlers.find(p.first);
		//if (it != _handlers.end()) {
			//NEX_DELETE((*it).second);
		//}
	}
}

} /* namespace nextar */
