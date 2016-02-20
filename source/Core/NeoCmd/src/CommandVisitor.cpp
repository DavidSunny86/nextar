/*
 * CommandVisitor.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#include <CommandVisitor.h>
#include <RootBlockCommandHandler.h>

namespace nextar {

CommandVisitor::CommandVisitor(CommandContext* context) : _context(context), popHandler(false) {
}

CommandVisitor::~CommandVisitor() {
}

void CommandVisitor::VisitDocumentBegin(const ASTDocument*) {
	const RootBlockCommandHandler* handler = _context->GetRoot();
	handler->BeginDocument(_context);
}

void CommandVisitor::VisitCommandBegin(const ASTCommand* command) {
	const CommandHandler* handler = _context->GetActiveHandler();
	if (handler && handler->IsBlockHandler()) {
		const CommandHandler* redirect = static_cast<BlockCommandHandler*>(handler)->GetHandler(command->GetName());
		if (redirect) {
			popHandler = true;
			_context->SetActiveHandler(redirect);
			redirect->BeginExecute(_context, command);
		}
	}
}


void CommandVisitor::VisitCommandEnd(const ASTCommand* command) {
	if (popHandler) {
		const CommandHandler* handler = _context->GetActiveHandler();
		handler->EndExecute(_context, command);
		_context->SetActiveHandler(handler->GetParentBlock());
	}
}

void CommandVisitor::VisitDocumentEnd(const ASTDocument*) {
	const RootBlockCommandHandler* handler = _context->GetRoot();
	handler->EndDocument(_context);
}

} /* namespace nextar */
