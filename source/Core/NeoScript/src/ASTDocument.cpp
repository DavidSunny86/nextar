/*
 * ASTDocument.cpp
 *
 *  Created on: 29-Oct-2015
 *      Author: obhi
 */

#include <ASTDocument.h>
#include <NeoScriptHeaders.h>

namespace nextar {

void nextar::ASTDocument::Accept(ASTVisitor* visitor) const {
	visitor->VisitDocumentBegin(this);
	for(auto& r : _regions) {
		r->Accept(visitor);
	}
	visitor->VisitDocumentEnd(this);
}

}


