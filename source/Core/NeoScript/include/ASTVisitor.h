/*
 * ASTVisitor.h
 *
 *  Created on: 26-Oct-2015
 *      Author: obhi
 */

#ifndef CORE_NEOSCRIPT_INCLUDE_ASTVISITOR_H_
#define CORE_NEOSCRIPT_INCLUDE_ASTVISITOR_H_

#include <NeoBaseDecl.h>

namespace nextar {

class _NexNeoScriptAPI ASTVisitor {
public:
	ASTVisitor() {}

	virtual void VisitDocumentBegin(const ASTDocument*) = 0;
	virtual void VisitDocumentEnd(const ASTDocument*) = 0;
	virtual void VisitCommandBegin(const ASTCommand*) = 0;
	virtual void VisitCommandEnd(const ASTCommand*) = 0;
	virtual void VisitBlockBegin(const ASTBlock*) = 0;
	virtual void VisitBlockEnd(const ASTBlock*) = 0;
	virtual void VisitBlockRegionBegin(const ASTBlockRegion*) = 0;
	virtual void VisitBlockRegionEnd(const ASTBlockRegion*) = 0;
	virtual void VisitTextRegionBegin(const ASTTextRegion*) = 0;
	virtual void VisitTextRegionEnd(const ASTTextRegion*) = 0;
	virtual void VisitParameter(const ASTParameter*) = 0;

protected:
	~ASTVisitor() {}
};

} /* namespace nextar */

#endif /* CORE_NEOSCRIPT_LOCAL_INCLUDE_ASTVISITOR_H_ */
