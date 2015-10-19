#ifndef ASTBLOCKREGION_H_
#define ASTBLOCKREGION_H_

#include <ASTRegion.h>
#include <ASTBlock.h>

namespace nextar {

class _NexNeoScriptAPI ASTBlockRegion : public ASTRegion {
	NEX_LOG_HELPER(ASTBlockRegion);

public:
	ASTBlockRegion(const String& name) : ASTRegion(name), _main(name) {}

	virtual const String& GetContents() const;
	virtual const ASTCommandList& GetCommands() const;
	virtual void AddCommands(ASTCommandList&& block);
	virtual void SetContents(String&& contents);

	virtual Type GetType() const {return Type::AST_BLOCK_REGION;}
private:
	// this block is not a declared block
	ASTBlock _main;
};

}

#endif // ASTBLOCKREGION_H_
