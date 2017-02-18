// Abhishek
// Nov 25 2007
//---------------------------
//## This is where I Start again, after 
//## realizing the mistakes of the frame
//## work.
#ifndef NEXTAR_BASE_DEF_H
#define NEXTAR_BASE_DEF_H

#include <StdHeaders.h>
#include <BaseConfig.h>
#include <NexAlloc.h>
#include <EnumMacros.h>

namespace nextar {

// standard types
typedef std::int8_t int8;
typedef std::int16_t int16;
typedef std::int32_t int32;
typedef std::int64_t int64;

typedef std::uint8_t uint8;
typedef std::uint16_t uint16;
typedef std::uint32_t uint32;
typedef std::uint64_t uint64;
typedef std::size_t hash64;

typedef uint8 byte;
typedef uint8 utf8;
typedef uint16 utf16;
typedef uint32 utf32;
typedef uint32 VersionID;
typedef uint32 EventID;

template<typename T, enum MemoryCategory cat = MEMCAT_GENERAL>
struct BasicUniquePtr {
	typedef std::unique_ptr<T, GenericFree<T, cat> > type;
};

template<typename T>
struct UniquePtr {
	typedef std::unique_ptr<T, GenericDelete<T> > type;
};

typedef BasicUniquePtr<uint8>::type DataPtr;

union Size {
	uint32 combined;
	struct {
		uint16 width, height;
	};
	struct {
		uint16 dx, dy;
	};

	inline Size() :
			combined(0) {
	}
	inline Size(uint16 a, uint16 b) :
			dx(a), dy(b) {
	}

	inline friend bool operator == (Size s1, Size s2) {
		return (s1.combined == s2.combined) != 0;
	}

	inline friend bool operator != (Size s1, Size s2) {
		return (s1.combined != s2.combined) != 0;
	}
};

union Point {
	uint32 combined;
	struct {
		int16 x, y;
	};
	inline Point() :
			combined(0) {
	}
	inline Point(int16 a, int16 b) :
			x(a), y(b) {
	}
};

// todo Does not belong here
enum class ColorMask
	: uint8 {
		// Color mask red
	MASK_RED = 1,
	// Color mask green
	MASK_GREEN = 2,
	// Color mask blue
	MASK_BLUE = 4,
	// Color mask alpha
	MASK_ALPHA = 8,
	// Color mask combined
	MASK_ALL = 0xff
};

NEX_ENUM_FLAGS(ColorMask, uint8);

// todo May be entirely useless
enum class Constants
	: uint32 {
	INVALID_VERSION_ID = 0xffffffff,
	INVALID_SIZE = 0xffffffff,
	MAX_LOADER_THREAD = 2,
};

namespace debug {
#ifdef NEX_DEBUG
// type_info only supported during debugging.
typedef const std::type_info& ConstTypeInfo;
#else
typedef const int32 ConstTypeInfo;
#endif
}

}

// Common functions
namespace nextar {
 // deriving from this class will generate
 // a compilation error on copy.
class NonCopyableObject {
public:

inline NonCopyableObject() {
}
private:
NonCopyableObject(const NonCopyableObject&);
const NonCopyableObject & operator =(const NonCopyableObject&);
};

	// compatibility test
	// check version compatibility
inline bool TestVersion(VersionID query, VersionID present) {
return ((NEX_GET_VER_MAJOR(present) == NEX_GET_VER_MAJOR(query))
		&& (NEX_GET_VER_MINREV(present) <= NEX_GET_VER_MINREV(query))) != 0;
}
}

#include <Containers.h>
#include <ErrorDefs.h>


namespace nextar {

struct StringID {
	StringID(const StringID& i) : _value(i._value) {}
	StringID() : _value(0) {}
	explicit StringID(hash64 v) : _value(v) {}

	friend inline bool operator < (const StringID first, const StringID second) {
		return (first._value < second._value) != 0;
	}
	friend inline bool operator > (const StringID first, const StringID second) {
		return (first._value > second._value) != 0;
	}
	friend inline bool operator == (const StringID first, const StringID second) {
		return (first._value == second._value) != 0;
	}

	friend inline bool operator != (const StringID first, const StringID second) {
		return (first._value != second._value) != 0;
	}

	hash64 _value;
};

}

#endif //NEXTAR_BASE_DEF_H//
