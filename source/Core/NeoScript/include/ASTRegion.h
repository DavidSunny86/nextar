#ifndef ASTREGION_H_
#define ASTREGION_H_

#include <ASTNode.h>

namespace nextar {

class ASTRegion : public ASTNode {
public:
  ASTRegion();
  ~ASTRegion();

  String GetRegionContents() const;

protected:


};

}

#endif // ASTREGION_H_
