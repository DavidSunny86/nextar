#ifndef ASTREGION_H_
#define ASTREGION_H_

#include <ASTNode.h>

namespace nextar {

class _NexNeoScriptAPI ASTRegion : public ASTNode {
public:
	ASTRegion(const String& name) {_value = name;}
	~ASTRegion();

	const String& GetName() const {return _value;}
	virtual const ASTCommandList& GetCommands() const = 0;
	virtual const String& GetContents() const = 0;
	virtual void AddCommands(ASTCommandList&& block) = 0;
	virtual void SetContents(String&& contents) = 0;

protected:

};

}

#endif // ASTREGION_H_
