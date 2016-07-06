#ifndef ASTBLOCK_H_
#define ASTBLOCK_H_

#include <ASTCommand.h>

namespace nextar {

class _NexNeoScriptAPI ASTBlock : public ASTCommand {
public:
	ASTBlock(const String& name) : ASTCommand(name) {}
	ASTBlock(const String& name, ASTParameter&& params) : ASTCommand(name,
			std::move(params)) {}
	ASTBlock(const String& name, ASTParameter&& params,
			ASTCommandList&& commands) : ASTCommand(name,
			std::move(params)), _commands(std::move(commands)) {}

	void SetCommands(ASTCommandList&& c) {
		_commands = std::move(c);
	}

	void AddCommand(ASTCommandPtr& c) {
		_commands.push_back(c);
	}

	const ASTCommandList& GetCommands() const {
		return _commands;
	}

	virtual Type GetType() const;
	virtual void Accept(ASTVisitor*) const;
private:
	ASTCommandList _commands;
};

}
#endif // ASTBLOCK_H_
