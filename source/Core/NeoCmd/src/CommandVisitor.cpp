/*
 * CommandVisitor.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#include <NeoCmd.h>
#include <CommandVisitor.h>
#include <BlockCommandHandler.h>
#include <RootBlockCommandHandler.h>
#include <CommandContext.h>

namespace nextar {

CommandVisitor::CommandVisitor(CommandContext* context) : _context(context), popHandler(false),_dontskip(true) {
}

CommandVisitor::~CommandVisitor() {
}

void CommandVisitor::VisitDocumentBegin(const ASTDocument* doc) {
	const RootBlockCommandHandler* handler = _context->GetRoot();
	handler->BeginDocument(_context, doc);
}

void CommandVisitor::VisitCommandBegin(const ASTCommand* command) {
	const CommandHandler* handler = _context->GetActiveHandler();
	if (handler && handler->IsBlockHandler()) {
		const CommandHandler* redirect = static_cast<const BlockCommandHandler*>(handler)->GetHandler(command->GetName());
		if (redirect) {
			popHandler = true;
			_context->SetActiveHandler(redirect);
			_dontskip = redirect->BeginExecute(_context, command);
			return;
		}
	}
	popHandler = false;
}


void CommandVisitor::VisitCommandEnd(const ASTCommand* command) {
	if (popHandler) {
		const CommandHandler* handler = _context->GetActiveHandler();
		handler->EndExecute(_context, command);
		_context->SetActiveHandler(handler->GetParentBlock());
		_dontskip = true;
	}
}

void CommandVisitor::VisitDocumentEnd(const ASTDocument* doc) {
	const RootBlockCommandHandler* handler = _context->GetRoot();
	handler->EndDocument(_context, doc);
}

} /* namespace nextar */
