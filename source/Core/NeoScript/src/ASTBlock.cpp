
#include <NeoScriptHeaders.h>
#include <ASTBlock.h>

namespace nextar {


ASTNode::Type ASTBlock::GetType() const {
	return Type::AST_BLOCK;
}

void ASTBlock::Accept(ASTVisitor* visitor) const {
	visitor->VisitCommandBegin(this);
	if(visitor->VisitBlockBegin(this)) {
		for(auto& c : _commands) {
			c->Accept(visitor);
		}
	}
	visitor->VisitBlockEnd(this);
	visitor->VisitCommandEnd(this);
}

}
