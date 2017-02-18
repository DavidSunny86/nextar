/*
 * BlockCommandHandler.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */
#include <NeoCmd.h>
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

void BlockCommandHandler::RegisterHandler(const String& qualifiedName,
		CommandHandler* iHandler) {
	StringPair p = StringUtils::Split(qualifiedName, '.');
	if (p.first.length() > 0) {
		CommandHandler* handler = GetHandler(p.first);
		if (handler->IsBlockHandler()) {
			static_cast<BlockCommandHandler*>(handler)->RegisterHandler(p.second, iHandler);
		}
	} else {
		auto& e = _handlers[p.second];
		if (e)
			e->SetParentBlock(nullptr);
		e = iHandler;
		e->SetParentBlock(this);
	}
}

void BlockCommandHandler::UnregisterHandler(const String& qualifiedName) {
	StringPair p = StringUtils::Split(qualifiedName, '.');
	if (p.first.length() > 0) {
		CommandHandler* handler = GetHandler(p.first);
		if (handler->IsBlockHandler()) {
			static_cast<BlockCommandHandler*>(handler)->UnregisterHandler(p.second);
		}
	} else {
		_handlers.erase(p.second);
		//auto it = _handlers.find(p.first);
		//if (it != _handlers.end()) {
			//NEX_DELETE((*it).second);
		//}
	}
}

} /* namespace nextar */
