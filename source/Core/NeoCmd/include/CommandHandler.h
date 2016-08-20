/*
 * CommandHandler.h
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_NEOCMD_INCLUDE_COMMANDHANDLER_H_
#define CORE_NEOCMD_INCLUDE_COMMANDHANDLER_H_

#include <NeoCmd.h>

namespace nextar {

class CommandContext;
class BlockCommandHandler;


class _NexNeoCmdAPI RegionHandler : public AllocGeneral {
public:
	virtual bool BeginExecute(CommandContext* pContext, const ASTRegion* region, bool isText) const { return true; }
	virtual void EndExecute(CommandContext* pContext, const ASTRegion* command, bool isText) const {}

	void RegisterHandler(const String& fullyQualifiedName, CommandHandler* pHandler);
	void UnregisterHandler(const String& fullyQualifiedName);

	RegionHandler(BlockCommandHandler* h) : _root(h) {}

	virtual ~RegionHandler();

	const BlockCommandHandler* GetRoot() const {
		return _root;
	}

protected:

	BlockCommandHandler* _root;
};

class _NexNeoCmdAPI CommandHandler : public AllocGeneral {
public:
	CommandHandler(const bool isBlock = false);
	virtual ~CommandHandler();

	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const { return true; }
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const {}

	void SetParentBlock(BlockCommandHandler* parent) {
		_parent = parent;
	}

	const BlockCommandHandler* GetParentBlock() const {
		return _parent;
	}

	bool IsBlockHandler() const {
		return _isBlockHandler;
	}

	virtual const CommandHandler* GetHandler(const String& name) const {
		return nullptr;
	}

protected:

	BlockCommandHandler* _parent;
	const bool _isBlockHandler;
};

typedef map<String, CommandHandler*>::type CommandHandlerMap;

} /* namespace nextar */


#endif /* CORE_NEOCMD_INCLUDE_COMMANDHANDLER_H_ */
