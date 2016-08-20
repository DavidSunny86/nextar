/*
 * CommandDictonaryArchive.h
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_NEOCMD_INCLUDE_COMMANDDICTIONARYARCHIVE_H_
#define CORE_NEOCMD_INCLUDE_COMMANDDICTIONARYARCHIVE_H_

#include <NeoCmd.h>
#include <Singleton.h>
#include <CommandDictionary.h>

namespace nextar {

class _NexNeoCmdAPI CommandDictionaryArchive : public Singleton<CommandDictionaryArchive> {
public:
	CommandDictionaryArchive();
	virtual ~CommandDictionaryArchive();

	CommandDictionary* RegisterDictionary(const String& name, RegionHandler* value);
	void UnregisterDictionary(const String& name);

	CommandDictionary* Get(const String& name);

protected:
	static CommandDictionaryArchive _instance;
	CommandDictionaryMap _scriptHandlers;
};


inline CommandDictionary* CommandDictionaryArchive::Get(const String& name) {
	auto it = _scriptHandlers.find(name);
	if (it != _scriptHandlers.end()) {
		return (*it).second;
	}
	return nullptr;
}

} /* namespace nextar */

#endif /* CORE_NEOCMD_INCLUDE_COMMANDDICTIONARYARCHIVE_H_ */
