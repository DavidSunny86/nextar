/*
 * ASTPrinter.h
 *
 *  Created on: 27-Oct-2015
 *      Author: obhi
 */

#ifndef CORE_NEOSCRIPT_LOCAL_INCLUDE_ASTPRINTER_H_
#define CORE_NEOSCRIPT_LOCAL_INCLUDE_ASTPRINTER_H_

#include <NeoBaseDecl.h>
#include <ASTVisitor.h>

namespace nextar {

class ASTPrinter: public ASTVisitor {
public:
	ASTPrinter(OutputStreamPtr& p);
	~ASTPrinter();

	virtual void VisitDocumentBegin(const ASTDocument*);
	virtual void VisitDocumentEnd(const ASTDocument*);
	virtual void VisitCommandBegin(const ASTCommand*);
	virtual void VisitCommandEnd(const ASTCommand*);
	virtual void VisitBlockBegin(const ASTBlock*);
	virtual void VisitBlockEnd(const ASTBlock*);
	virtual void VisitBlockRegionBegin(const ASTBlockRegion*);
	virtual void VisitBlockRegionEnd(const ASTBlockRegion*);
	virtual void VisitTextRegionBegin(const ASTTextRegion*);
	virtual void VisitTextRegionEnd(const ASTTextRegion*);
	virtual void VisitParameter(const ASTParameter*);

protected:

	OutStringStream& Indent() {
		std::fill_n(std::ostream_iterator<char>(_out), _indention, '\t');
		return _out;
	}
	uint32 _indention;
	OutStringStream _out;
	OutputStreamPtr _outputStream;
};

} /* namespace nextar */

#endif /* CORE_NEOSCRIPT_LOCAL_INCLUDE_ASTPRINTER_H_ */
