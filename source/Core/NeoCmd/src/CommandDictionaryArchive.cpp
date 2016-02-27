/*
 * CommandDictonaryArchive.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#include <NeoCmd.h>
#include <CommandDictionaryArchive.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(CommandDictionaryArchive);

CommandDictionaryArchive CommandDictionaryArchive::_instance;

CommandDictionaryArchive::CommandDictionaryArchive() {

}

CommandDictionaryArchive::~CommandDictionaryArchive() {
	for(auto &e : _scriptHandlers) {
		NEX_DELETE(e.second);
	}
}

CommandDictionary* CommandDictionaryArchive::RegisterDictionary(
		const String& name, RootBlockCommandHandler* value) {
	CommandDictionary* dict = NEX_NEW(CommandDictionary(name, value));
	_scriptHandlers[name] = dict;
	return dict;
}

void CommandDictionaryArchive::UnregisterDictionary(const String& name) {
	auto it = _scriptHandlers.find(name);
	if (it != _scriptHandlers.end()) {
		NEX_DELETE((*it).second);
		_scriptHandlers.erase(it);
	}
}

} /* namespace nextar */
