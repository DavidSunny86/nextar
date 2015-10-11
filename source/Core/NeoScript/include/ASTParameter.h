#ifndef ASTPARAMETER_H_
#define ASTPARAMETER_H_

#include <ASTNode.h>

namespace nextar {

class ASTParameter : public ASTNode {
public:
  const String& GetStringValue() {
    return GetValue();
  }
};


}

#endif // ASTPARAMETER_H_
