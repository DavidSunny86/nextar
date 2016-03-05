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

	virtual void BeginDocument(CommandContext* pContext, const ASTDocument* pDoc) const {}
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const { return true; }
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const {}
	virtual void EndDocument(CommandContext* pContext, const ASTDocument* pDoc) const {}
};

} /* namespace nextar */

#endif /* CORE_NEOCMD_LOCAL_INCLUDE_ROOTBLOCKCOMMANDHANDLER_H_ */
