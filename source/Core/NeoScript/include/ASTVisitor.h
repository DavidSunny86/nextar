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
	virtual bool VisitBlockBegin(const ASTBlock*) = 0; // return false to skip parsing the block
	virtual void VisitBlockEnd(const ASTBlock*) = 0;
	virtual void VisitBlockRegionBegin(const ASTBlockRegion*) = 0;
	virtual void VisitBlockRegionEnd(const ASTBlockRegion*) = 0;
	virtual void VisitTextRegionBegin(const ASTTextRegion*) = 0;
	virtual void VisitTextRegionEnd(const ASTTextRegion*) = 0;


protected:
	~ASTVisitor() {}
};

class _NexNeoScriptAPI ASTVisitorAdapter : public ASTVisitor {
public:
	ASTVisitorAdapter() {}

	virtual void VisitDocumentBegin(const ASTDocument*) {}
	virtual void VisitDocumentEnd(const ASTDocument*) {}
	virtual void VisitCommandBegin(const ASTCommand*) {}
	virtual void VisitCommandEnd(const ASTCommand*) {}
	virtual bool VisitBlockBegin(const ASTBlock*) { return true; }
	virtual void VisitBlockEnd(const ASTBlock*) {}
	virtual void VisitBlockRegionBegin(const ASTBlockRegion*) {}
	virtual void VisitBlockRegionEnd(const ASTBlockRegion*) {}
	virtual void VisitTextRegionBegin(const ASTTextRegion*) {}
	virtual void VisitTextRegionEnd(const ASTTextRegion*) {}


protected:
	~ASTVisitorAdapter() {}
};

} /* namespace nextar */

#endif /* CORE_NEOSCRIPT_LOCAL_INCLUDE_ASTVISITOR_H_ */
