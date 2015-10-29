#ifndef ASTCOMMAND_H_
#define ASTCOMMAND_H_

#include <ASTNode.h>
#include <ASTParameterList.h>

namespace nextar {

class _NexNeoScriptAPI ASTCommand : public ASTNode {
public:
	ASTCommand(const String& name) : ASTNode(name) {}
	ASTCommand(const String& name, ASTParameterList&& params) : ASTNode(name),
	_parameters(std::move(params)) {}

	const String& GetName() const {
		return GetValue();
	}
	
	virtual Type GetType() const {return Type::AST_COMMAND;}
	virtual void Accept(ASTVisitor*) const;
private:
	ASTParameterList _parameters;
};

}

#endif // ASTCOMMAND_H_
