#ifndef ASTTEXTREGION_H_
#define ASTTEXTREGION_H_
#include <ASTRegion.h>

namespace nextar {

class _NexNeoScriptAPI ASTTextRegion : public ASTRegion {
	NEX_LOG_HELPER(ASTTextRegion);
public:

	ASTTextRegion(const String& name) : ASTRegion(name) {}

	virtual const String& GetContents() const;
	virtual const ASTCommandList& GetCommands() const;
	virtual void AddCommands(ASTCommandList&& block);
	virtual void SetContents(String&& contents);

	virtual Type GetType() const {return Type::AST_TEXT_REGION;}
	virtual void Accept(ASTVisitor*) const;
public:
	String _contents;
};

}

#endif // ASTTEXTREGION_H_
