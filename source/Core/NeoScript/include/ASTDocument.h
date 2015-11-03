#ifndef ASTDOCUMENT_H_
#define ASTDOCUMENT_H_

#include <ASTNode.h>

namespace nextar {

class _NexNeoScriptAPI ASTDocument : public ASTNode {
public:
	ASTDocument(String name) : ASTNode(name) {}

	const String& GetName() const {
		return GetValue();
	}
	
	void AddRegion(ASTRegionPtr& reg) {
		_regions.push_back(reg);
	}

	void AddRegionFront(ASTRegionPtr& reg) {
		_regions.push_front(reg);
	}

	const ASTRegionList& GetRegions() const {
		return _regions;
	}

	virtual Type GetType() const {return Type::AST_DOCUMENT;}
	virtual void Accept(ASTVisitor*) const;
private:

	ASTRegionList _regions;
};

}


#endif // ASTDOCUMENT_H_
