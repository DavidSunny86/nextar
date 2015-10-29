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
	StringUtils::ConstMultiString m(_value);
	auto it =  m.Iterate();
	ASTParameter temp;
	while(it.HasNext(temp.AsStringRef())) {
		temp.Accept(visitor);
	}
}

}



