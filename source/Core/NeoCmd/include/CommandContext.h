/*
 * CommandContext.h
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_NEOCMD_INCLUDE_COMMANDCONTEXT_H_
#define CORE_NEOCMD_INCLUDE_COMMANDCONTEXT_H_

#include <NeoCmd.h>

namespace nextar {

class RootBlockCommandHandler;
class _NexNeoCmdAPI CommandContext {
public:
	CommandContext();
	virtual ~CommandContext();

	inline const CommandDictionary* GetDictionary() const {
		return _dictionary;
	}

	inline const CommandHandler* GetActiveHandler() const {
		return _activeHandler;
	}

	inline void SetActiveHandler(const CommandHandler* handler) {
		_activeHandler = handler;
	}

	inline const RootBlockCommandHandler* GetRoot() const {
		return _dictionary->GetRoot();
	}

protected:
	friend class NeoCommandInterpreter;


	inline void SetDictionary(CommandDictionary* dic) {
		_dictionary = dic;
		_activeHandler = dic->GetRoot();
	}

	CommandDictionary* _dictionary;
	const CommandHandler* _activeHandler;
};

} /* namespace nextar */

#endif /* CORE_NEOCMD_INCLUDE_COMMANDCONTEXT_H_ */
