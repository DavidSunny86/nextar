
#ifndef ASTDOCUMENT_H_
#define ASTDOCUMENT_H_

#include <ASTNode.h>

namespace nextar {

class ASTDocument : public ASTNode {

private:
  ASTRegionList _regions;
};

}

#endif // ASTDOCUMENT_H_
