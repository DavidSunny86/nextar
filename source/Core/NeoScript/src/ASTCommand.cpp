/*
 * ASTCommand.cpp
 *
 *  Created on: 29-Oct-2015
 *      Author: obhi
 */
#include <ASTCommand.h>
#include <NeoScriptHeaders.h>

namespace nextar {

void ASTCommand::Accept(ASTVisitor* visitor) const {
	visitor->VisitCommandBegin(this);
	_parameters.Accept(visitor);
	visitor->VisitCommandEnd(this);
}

}



