#ifndef ASTPARAMETER_H_
#define ASTPARAMETER_H_

#include <ASTNode.h>

namespace nextar {

class _NexNeoScriptAPI ASTParameter : public AllocGeneral {
public:

	ASTParameter() {}
	ASTParameter(const String& value) : _value(value) {}
	ASTParameter(const ASTParameter& p) : _value(p._value) {}
	ASTParameter(ASTParameter&& p) : _value(std::move(p._value)) {}

	ASTParameter& operator = (const ASTParameter& p) {
		_value = p._value;
		return *this;
	}
	ASTParameter& operator = (ASTParameter&& p) {
		_value = std::move(p._value);
		return *this;
	}

	const String& AsString() const {
		return _value;
	}

	void Append(const String& value);
	void Append(const ASTParameter& value);

protected:
	String _value;

};

}

#endif // ASTPARAMETER_H_
