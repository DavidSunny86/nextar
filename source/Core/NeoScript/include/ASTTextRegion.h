#ifndef ASTTEXTREGION_H_
#define ASTTEXTREGION_H_
#include <ASTRegion.h>

namespace nextar {

class ASTTextRegion : public ASTRegion {
public:
  String _contents;
};

}

#endif // ASTTEXTREGION_H_
