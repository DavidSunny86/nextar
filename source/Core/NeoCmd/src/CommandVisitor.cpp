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

CommandVisitor::CommandVisitor(CommandContext* context) : _context(context), popHandler(false), _dontskip(true), _skipReg(false) {
}

CommandVisitor::~CommandVisitor() {
}

void CommandVisitor::VisitDocumentBegin(const ASTDocument* doc) {
	const RootBlockCommandHandler* handler = _context->GetRoot();
	handler->BeginDocument(_context, doc);
}

void CommandVisitor::VisitCommandBegin(const ASTCommand* command) {
	if (_skipReg)
		return;
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
	if (_skipReg)
		return;
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

void CommandVisitor::VisitBlockRegionBegin(const ASTBlockRegion* reg) {
	if (reg->GetName() != StringUtils::Null) {
		const RegionHandler* h = _context->GetRegionHandler(GetRefinedRegionName(reg->GetName()));
		if (h) {
			// @todo potential error, if no handler is found, we have no way to switch
			// back dictionaries
			_context->SetActiveRegionHandler(h);
			_skipReg = h->BeginExecute(_context, reg, false);
		} else
			_skipReg = true;
	}
		
}

void CommandVisitor::VisitBlockRegionEnd(const ASTBlockRegion* reg) {
	const RegionHandler* r = _context->GetActiveRegionHandler();
	if (r)
		r->EndExecute(_context, reg, true);
	_context->SetActiveRegionHandler(nullptr);
}

void CommandVisitor::VisitTextRegionBegin(const ASTTextRegion* reg) {
	if (reg->GetName() != StringUtils::Null) {
		const RegionHandler* h = _context->GetRegionHandler(GetRefinedRegionName(reg->GetName()));
		if (h) {
			// @todo potential error, if no handler is found, we have no way to switch
			// back dictionaries
			_context->SetActiveRegionHandler(h);
			_skipReg = h->BeginExecute(_context, reg, true);
		} else {
			_skipReg = true;
		}
	}
}

void CommandVisitor::VisitTextRegionEnd(const ASTTextRegion* reg) {
	const RegionHandler* r = _context->GetActiveRegionHandler();
	if (r)
		r->EndExecute(_context, reg, true);
	_context->SetActiveRegionHandler(nullptr);
}

String CommandVisitor::GetRefinedRegionName(const String& n) {
	size_t p = n.find_first_of('.');
	if (p != String::npos) {
		return n.substr(0, p);
	}
	return n;
}

} /* namespace nextar */
