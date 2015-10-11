#ifndef ASTBLOCK_H_
#define ASTBLOCK_H_

#include <ASTCommand.h>

namespace nextar {

class ASTBlock : public ASTCommand {

private:
  ASTCommandList _commands;
};

}
#endif // ASTBLOCK_H_
