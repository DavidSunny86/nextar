/*
 * RootBlockCommand.h
 *
 *  Created on: 13-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_NEOCMD_LOCAL_INCLUDE_ROOTBLOCKCOMMANDHANDLER_H_
#define CORE_NEOCMD_LOCAL_INCLUDE_ROOTBLOCKCOMMANDHANDLER_H_

#include <BlockCommandHandler.h>

namespace nextar {

class _NexNeoCmdAPI RootBlockCommandHandler: public BlockCommandHandler {
public:
	RootBlockCommandHandler();
	virtual ~RootBlockCommandHandler();

	virtual void BeginDocument(CommandContext* pContext, const ASTDocument* pDoc) const = 0;
	virtual void BeginExecute(CommandContext* pContext, const ASTCommand* command) const {}
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const {}
	virtual void EndDocument(CommandContext* pContext, const ASTDocument* pDoc) const = 0;
};

} /* namespace nextar */

#endif /* CORE_NEOCMD_LOCAL_INCLUDE_ROOTBLOCKCOMMANDHANDLER_H_ */
