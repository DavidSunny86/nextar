#include <NeoBaseDecl.h>
#include <NeoScriptHeaders.h>
#include <ASTBlockRegion.h>

namespace nextar {

const String& ASTBlockRegion::GetContents() const {
	Error("GetContents - Incorrect function call!");
	return StringUtils::Null;
}

void ASTBlockRegion::AddCommands(ASTCommandList&& block) {
	_main.SetCommands(std::move(block));
}

void ASTBlockRegion::SetContents(String&& contents) {
	Error("SetContents - Incorrect function call!");
}

const ASTCommandList& ASTBlockRegion::GetCommands() const {
	return _main.GetCommands();
}

void ASTBlockRegion::Accept(ASTVisitor* visitor) const {
	visitor->VisitBlockRegionBegin(this);
	const ASTCommandList& commands = GetCommands();
	for (auto& c : commands)
		c->Accept(visitor);
	visitor->VisitBlockRegionEnd(this);
}

}
