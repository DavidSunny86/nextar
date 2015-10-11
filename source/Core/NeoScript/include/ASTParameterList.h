#ifndef ASTPARAMETERLIST_H_
#define ASTPARAMETERLIST_H_

#include <ASTParameter.h>

namespace nextar {

class ASTParameterList : public ASTParameter {
public:
  const String& GetStringValue() {
    return GetValue();
  }
};


}

#endif // ASTPARAMETERLIST_H_
