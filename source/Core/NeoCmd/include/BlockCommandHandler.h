/*
 * BlockCommandHandler.h
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_NEOCMD_INCLUDE_BLOCKCOMMANDHANDLER_H_
#define CORE_NEOCMD_INCLUDE_BLOCKCOMMANDHANDLER_H_

#include <CommandHandler.h>

namespace nextar {

class BlockCommandHandler : public CommandHandler {
public:
	BlockCommandHandler();
	virtual ~BlockCommandHandler();

	void AddHandler(const String& qualifiedName,
			CommandHandler* handler);
	void RemoveHandler(const String& qualifiedName);

	virtual const CommandHandler* GetHandler(const String& name) const;

protected:

	CommandHandler* GetHandler(const String& name) {
		auto it = _handlers.find(name);
		if (it != _handlers.end()) {
			return (*it).second;
		}
		return nullptr;
	}

	CommandHandlerMap _handlers;

};

} /* namespace nextar */

#endif /* CORE_NEOCMD_INCLUDE_BLOCKCOMMANDHANDLER_H_ */
