#ifndef ASTCOMMAND_H_
#define ASTCOMMAND_H_

#include <ASTNode.h>
#include <ASTParameter.h>

namespace nextar {

class _NexNeoScriptAPI ASTCommand : public ASTNode {
public:
	ASTCommand(const String& name) : ASTNode(name) {}
	ASTCommand(const String& name, ASTParameter&& params) : ASTNode(name),
	_parameters(std::move(params)) {}

	const String& GetName() const {
		return GetValue();
	}
	
	const ASTParameter& GetParameters() const {
		return _parameters;
	}

	virtual Type GetType() const {return Type::AST_COMMAND;}
	virtual void Accept(ASTVisitor*) const;
private:
	ASTParameter _parameters;
};

}

#endif // ASTCOMMAND_H_
