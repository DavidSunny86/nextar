/*
 * CommandDictionary.h
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_NEOCMD_INCLUDE_COMMANDDICTIONARY_H_
#define CORE_NEOCMD_INCLUDE_COMMANDDICTIONARY_H_

#include <BlockCommandHandler.h>

namespace nextar {

typedef map<String, RegionHandler*>::type RegionHandlerMap;

class _NexNeoCmdAPI CommandDictionary : public AllocGeneral {
public:
	CommandDictionary(const String& name, RegionHandler* unnamedRegionHandler);
	virtual ~CommandDictionary();

	void RegisterRegionHandler(const String& fullyQualifiedName, RegionHandler* pHandler);
	void UnregisterRegionHandler(const String& fullyQualifiedName);

	inline const RegionHandler* GetRegionHandler(const String& name) const {
		auto it = _regionHandlers.find(name);
		return (it != _regionHandlers.end()) ? (*it).second : nullptr;
	}

protected:

	RegionHandlerMap _regionHandlers;
	String _name;
};

typedef map<String, CommandDictionary*>::type CommandDictionaryMap;

} /* namespace nextar */

#endif /* CORE_NEOCMD_INCLUDE_COMMANDDICTIONARY_H_ */
