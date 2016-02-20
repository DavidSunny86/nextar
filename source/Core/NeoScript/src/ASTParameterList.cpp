/*
 * ASTParameterList.cpp
 *
 *  Created on: 29-Oct-2015
 *      Author: obhi
 */
#include <ASTParameterList.h>
#include <NeoScriptHeaders.h>

namespace nextar {

void ASTParameterList::Accept(ASTVisitor* visitor) const {
	if(_value.size() > 0)
		visitor->VisitParameterList(this);
}

}



