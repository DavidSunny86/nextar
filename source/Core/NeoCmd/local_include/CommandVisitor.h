/*
 * CommandVisitor.h
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_NEOCMD_COMMANDVISITOR_H_
#define CORE_NEOCMD_COMMANDVISITOR_H_

#include <ASTVisitor.h>

namespace nextar {

class CommandVisitor: public ASTVisitorAdapter {
public:
	CommandVisitor(CommandContext* context);
	virtual ~CommandVisitor();

	virtual void VisitDocumentBegin(const ASTDocument*);
	virtual void VisitDocumentEnd(const ASTDocument*);

	virtual void VisitCommandBegin(const ASTCommand* command);
	virtual void VisitCommandEnd(const ASTCommand* command);

protected:
	bool popHandler;
	CommandContext* _context;
};

} /* namespace nextar */

#endif /* CORE_NEOCMD_COMMANDVISITOR_H_ */
