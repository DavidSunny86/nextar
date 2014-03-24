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

	typedef uint8 byte;
	typedef uint8 utf8;
	typedef uint16 utf16;
	typedef uint32 utf32;
	typedef uint32 VersionID;
	typedef uint8 ColorMaskType;

	typedef std::unique_ptr<uint8, GenericDeleter<uint8> > DataPtr;

	union Size {
		uint32 combined;
		struct {
			uint16 width, height;
		};
		struct {
			uint16 dx, dy;
		};

		inline Size(uint16 a, uint16 b) : dx(a), dy(b) {}
	};

	union Point {
		uint32 combined;
		struct {
			int16 x, y;
		};
		inline Point(int16 a, int16 b) : x(a), y(b) {}
	};

	enum ColorMask {
		// Color mask red
		COLOR_MASK_RED = 1,
		// Color mask green
		COLOR_MASK_GREEN = 2,
		// Color mask blue
		COLOR_MASK_BLUE = 4,
		// Color mask alpha
		COLOR_MASK_ALPHA = 8,
		// Color mask combined
		COLOR_MASK_ALL = 0xff
	};

	enum Constants {
		CONST_INVALID_VERSION_ID = -1, CONST_INVALID_SIZE = -1,
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
		&& (NEX_GET_VER_MINREV(present) <= NEX_GET_VER_MINREV(query)))!= 0;
	}
}

#include <Containers.h>
#include <ErrorDefs.h>

#endif //NEXTAR_BASE_DEF_H
//
