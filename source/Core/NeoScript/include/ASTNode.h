#ifndef ASTNODE_H_
#define ASTNODE_H_

#include <NeoBaseDecl.h>

namespace nextar {

class _NexNeoScriptAPI ASTNode : public Referenced<ASTNode, AllocGeneral> {
public:
	enum Type {
		AST_NODE,
		AST_DOCUMENT,
		AST_COMMAND,
		AST_BLOCK,
		AST_BLOCK_REGION,
		AST_TEXT_REGION,
	};

	ASTNode() {}
	ASTNode(const String& n) : _value(n) {}
	virtual ~ASTNode() {}

	void SetValue(const String& v) {
		_value = v;
	}

	const String& GetValue() {
		return _value;
	}

	virtual Type GetType() const = 0;

protected:

	String _value;
};

}

#endif // ASTNODE_H_
