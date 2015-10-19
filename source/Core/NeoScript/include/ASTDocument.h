#ifndef ASTDOCUMENT_H_
#define ASTDOCUMENT_H_

#include <ASTNode.h>

namespace nextar {

class _NexNeoScriptAPI ASTDocument : public ASTNode {
public:
	ASTDocument(String name) : _name(name) {}

	void AddRegion(ASTRegionPtr& reg) {
		_regions.push_back(reg);
	}

	const ASTRegionList& GetRegions() const {
		return _regions;
	}

	virtual Type GetType() const {return Type::AST_DOCUMENT;}
private:
	String _name;
	ASTRegionList _regions;
};

}

#endif // ASTDOCUMENT_H_
