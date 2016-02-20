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

class _NexNeoCmdAPI CommandDictionary : public AllocGeneral {
public:
	CommandDictionary(const String& name, RootBlockCommandHandler* root);
	virtual ~CommandDictionary();

	void RegisterHandler(const String& fullyQualifiedName, CommandHandler* pHandler);
	void UnregisterHandler(const String& fullyQualifiedName);

	inline const RootBlockCommandHandler* GetRoot() const {
		return _root;
	}
protected:

	RootBlockCommandHandler* _root;
	String _name;
};

typedef map<String, CommandDictionary*>::type CommandDictionaryMap;

} /* namespace nextar */

#endif /* CORE_NEOCMD_INCLUDE_COMMANDDICTIONARY_H_ */
