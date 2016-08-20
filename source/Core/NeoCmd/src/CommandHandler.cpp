/*
 * CommandHandler.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#include <NeoCmd.h>
#include <CommandHandler.h>

namespace nextar {

CommandHandler::CommandHandler(const bool isBlock) : _isBlockHandler(isBlock),
		_parent(nullptr) {
}

CommandHandler::~CommandHandler() {
}

RegionHandler::~RegionHandler() {
}


void RegionHandler::RegisterHandler(const String& fullyQualifiedName,
	CommandHandler* handler) {
	_root->RegisterHandler(fullyQualifiedName, handler);
}

void RegionHandler::UnregisterHandler(const String& fullyQualifiedName) {
	_root->UnregisterHandler(fullyQualifiedName);
}

} /* namespace nextar */
