#include <ASTParameter.h>
#include <NeoScriptHeaders.h>

namespace nextar {

ASTParameter::DefaultResolver ASTParameter::DefaultResolver::_instance;

void ASTParameter::Append(const String& name) {
	MultiStringHelper msh(_value);
	msh.PushBack(name);
}

void ASTParameter::Append(const ASTParameter& p) {
	MultiStringHelper msh(_value);
	msh.PushBack(p._value);
}

void ASTParameter::AppendList(const ASTParameter& p) {
	String value;
	MultiStringHelper msh(_value);
	ConstMultiStringHelper src(p._value);

	if (src.IsSimpleString())
		msh.PushBack(p._value);
	else {
		if (src.Length() == 1)
			msh.PushBack(src.Get(0));
		else
			msh.PushBack(p._value);
	}
}

void ASTParameter::AppendExpanded(const ASTParameter& p) {
	String value;
	MultiStringHelper msh(_value);
	auto it = ConstMultiStringHelper::It(p._value);
	while (it.HasNext(value))
		msh.PushBack(value);
}

bool ASTParameter::Find(const String & name, String & value, const Resolver * pRes) const {
	ConstMultiStringHelper h(_value);
	auto it = h.Iterate();
	String v;
	while (it.HasNext(v)) {
		if (StringUtils::IsTagged(v, name, value)) {
			if (pRes)
				pRes->Resolve(value);
			return true;
		}
	}
	return false;
}

void ASTParameter::SetName(const String& name) {
	MultiStringHelper h(_value);
	String psuedoV = name + ":" + h.Get(0);
	_value.swap(psuedoV);
}

}
