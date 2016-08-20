/*
 * CommandDictionary.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */
#include <NeoCmd.h>
#include <CommandDictionary.h>
#include <BlockCommandHandler.h>

namespace nextar {

CommandDictionary::CommandDictionary(const String& name, RegionHandler* root) : _name(name) {
	if (root)
		RegisterRegionHandler("", root);
}

CommandDictionary::~CommandDictionary() {
}

void CommandDictionary::RegisterRegionHandler(const String& fullyQualifiedName, 
	RegionHandler* pHandler) {
	_regionHandlers[fullyQualifiedName] = pHandler;
}

void CommandDictionary::UnregisterRegionHandler(const String& fullyQualifiedName) {
	_regionHandlers.erase(fullyQualifiedName);
}

} /* namespace nextar */
