#include <ASTParameter.h>

namespace nextar {

void ASTParameter::Append(const String& name) {
	MultiStringHelper msh(_value);
	msh.PushBack(name);
}

void ASTParameter::Append(const ASTParameter& p) {
	MultiStringHelper msh(_value);
	msh.PushBack(p._value);
}

}
