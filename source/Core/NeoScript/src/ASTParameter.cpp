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

void ASTParameter::AppendExpanded(const ASTParameter& p) {
	String value;
	MultiStringHelper msh(_value);
	auto it = ConstMultiStringHelper::It(p._value);
	while (it.HasNext(value))
		msh.PushBack(value);
}

void ASTParameter::SetName(const String& name) {
	MultiStringHelper h(_value);
	String psuedoV = name + ":" + h.Get(0);
	_value.swap(psuedoV);
}

bool ASTParameter::Find(const String& name, String& value) const {
	ConstMultiStringHelper h(AsString());
	auto it = h.Iterate();
	while (it.HasNext(value)) {
		if (StringUtils::IsTagged(_value, name, value))
			return true;
	}
	return false;
}

}
