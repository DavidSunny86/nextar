#ifndef ASTNODE_H_
#define ASTNODE_H_

#include <NeoBaseDecl.h>

namespace nextar {

class ASTNode : public AllocGeneral {
public:
  ASTNode() {}
  virtual ~ASTNode() {}

  void SetValue(const String& v) {
    _value = v;
  }

  const String& GetValue() {
    return _value;
  }

protected:
  String _value;
};

}

#endif // ASTNODE_H_
