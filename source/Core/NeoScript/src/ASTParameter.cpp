#include <ASTParameter.h>
#include <NeoScriptHeaders.h>

namespace nextar {

void ASTParameter::Append(const String& name) {
	MultiStringHelper msh(_value);
	msh.PushBack(name);
}

void ASTParameter::Append(const ASTParameter& p) {
	MultiStringHelper msh(_value);
	msh.PushBack(p._value);
}

void ASTParameter::Accept(ASTVisitor* visitor) const {
	visitor->VisitParameter(this);
}

}
