#include <NeoBaseDecl.h>
#include <ASTTextRegion.h>
#include <NeoScriptHeaders.h>

namespace nextar {

const String& ASTTextRegion::GetContents() const {
	return _contents;
}

void ASTTextRegion::AddCommands(ASTCommandList&& block) {
	Error("AddBlock - Incorrect function call!");
}

void ASTTextRegion::SetContents(String&& contents) {
	_contents = std::move(contents);
}

const ASTCommandList& ASTTextRegion::GetCommands() const {
	Error("ASTTextRegion - Incorrect function call!");
	return *static_cast<const ASTCommandList*>(nullptr);
}

void ASTTextRegion::Accept(ASTVisitor* visitor) const {
	visitor->VisitTextRegionBegin(this);
	visitor->VisitTextRegionEnd(this);
}

}
