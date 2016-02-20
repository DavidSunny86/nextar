/*
 * CommandDictonaryArchive.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

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
	CommandDictionary* dict = NEX_NEW(CommandDictionary(value));
	_scriptHandlers[name] = dict;
	return dict;
}

void CommandDictionaryArchive::UnregisterDictionary(const String& name) {
	_scriptHandlers.erase(name);
}

} /* namespace nextar */
