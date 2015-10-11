#ifndef MULTISTRINGLIST_H_
#define MULTISTRINGLIST_H_

#include <StringUtils.h>

namespace nextar {
namespace StringUtils {

template <typename StringType>
class MultiStringList {

  uint32 Length() const {

  }

  uint32 IndexOf(const StringType& what) const {

  }

  StringType& Get(StringType& store, uint32 i) const {

  }

  void Insert(uint32 i, const StringType& what) {

  }

  void Replace(uint32 i, const StringType& what) {

  }
  
  void PushBack(const String& what) {

  }

  void PushFront(const String& what) {

  }

  StringType Get(uint32 i) const {
    StringType what;
    Get(what, i);
    return what;
  }


private:
  enum {
    // x> string x<
    STX_CHAR = 02,
    ETX_CHAR = 03,
  };
  StringType& _object;
};

}
}

#endif // MULTISTRINGLIST_H_
