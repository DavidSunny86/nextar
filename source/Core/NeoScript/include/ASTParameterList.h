#ifndef ASTPARAMETERLIST_H_
#define ASTPARAMETERLIST_H_

#include <ASTParameter.h>

namespace nextar {

class _NexNeoScriptAPI ASTParameterList : public ASTParameter {
public:
	ASTParameterList() {}
	ASTParameterList(const ASTParameterList& p) : ASTParameter(p.AsString()) {}
	ASTParameterList(ASTParameterList&& p) : ASTParameter(std::move(p._value)) {}

	ASTParameterList& operator = (const ASTParameterList& p) {
		_value = p._value;
		return *this;
	}

	ASTParameterList& operator = (ASTParameterList&& p) {
		_value = std::move(p._value);
		return *this;
	}
};

}

#endif // ASTPARAMETERLIST_H_
