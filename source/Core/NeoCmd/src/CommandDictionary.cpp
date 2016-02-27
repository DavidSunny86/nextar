/*
 * CommandDictionary.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */
#include <NeoCmd.h>
#include <CommandDictionary.h>
#include <RootBlockCommandHandler.h>

namespace nextar {

CommandDictionary::CommandDictionary(const String& name, RootBlockCommandHandler* root) : _name(name),
	_root(root) {
}

CommandDictionary::~CommandDictionary() {
}

void CommandDictionary::RegisterHandler(const String& fullyQualifiedName,
		CommandHandler* handler) {
	_root->AddHandler(fullyQualifiedName, handler);
}

void CommandDictionary::UnregisterHandler(const String& fullyQualifiedName) {
	_root->RemoveHandler(fullyQualifiedName);
}

} /* namespace nextar */
