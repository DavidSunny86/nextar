#ifndef ASTCOMMAND_H_
#define ASTCOMMAND_H_

#include <ASTNode.h>

namespace nextar {

class ASTCommand : public ASTNode {
public:
private:
  ASTParameterList* _parameters;
};



}

#endif // ASTCOMMAND_H_
