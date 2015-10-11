#ifndef ASTBLOCKREGION_H_
#define ASTBLOCKREGION_H_


#include <ASTRegion.h>
#include <ASTBlock.h>

namespace nextar {

class ASTBlockRegion : public ASTRegion {
public:
private:
  // this block is not a declared block
  ASTBlock* _main;
};

}

#endif // ASTBLOCKREGION_H_
