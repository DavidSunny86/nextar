#ifndef NEXTAR_NEXENDIN_ADJUSTMENTS
#define NEXTAR_NEXENDIN_ADJUSTMENTS

#include <NexBase.h>
#include <NexBase.h>

#if ( (NEX_STORE_IN_LITTLEENDIN) && (NEX_ENDIANNESS == NEX_ENDIAN_BIG) ) || ( !(NEX_STORE_IN_LITTLEENDIN) && (NEX_ENDIANNESS == NEX_ENDIAN_LITTLE) )
/** @remarks Use this macro to determine weather to swap bits if the 
 *			 object is serialized.
 **/
#   define NEX_NEED_ENDIAN_SWAPPING 1
#else
#   define NEX_NEED_ENDIAN_SWAPPING 0
#endif

namespace nextar {
	/** @remarks Endianness control.
	 * Every file could use little-endin mode to store items by default.
	 * The following routines assume that it is intended to store items
	 * in little endin mode; it is best if NEX_DATA_STORED_IN_# macro is not
	 * set and the default is used, as there is a consistency issue
	 * associated with it. if however the macro is modified, all files
	 * previously created should be modified and adjusted to the right
	 * mode; however those files which used a specific format to store
	 * items need not be recreated.

	 * Note: The vfs does not takes care of adjusting the endian-ness
	 * for data. The serializer does.
	 * --This is an update from the last time when it actually
	 * stored and loaded data and corrected endian-ness
	 * The function bellow must be used explicitly.
	 **/
	namespace Endian {

		enum Endianess {
			ENDIAN_BIG,
		 	ENDIAN_LITTLE
		};

		_NexInline uint16 Swap16(uint16 u) {
#if  defined(NEX_MSVC) && (_MSC_VER >= 1400)
			return _byteswap_ushort(u);
#else
			return (u >> 8) | (u << 8);
#endif
		}

		_NexInline uint32 Swap32(uint32 u) {
#if  defined(NEX_MSVC) && (_MSC_VER >= 1300)
			return _byteswap_ulong(u);
#else
			return (u >> 24) | ((u >> 8) & 0xff00) | ((u << 8) & 0xff0000)
					| (u << 24);
#endif
		}

		_NexInline uint64 Swap64(uint64 u) {
#if  defined(NEX_MSVC) && (_MSC_VER >= 1300)
			return _byteswap_uint64(u);
#else

			union {
				uint64 v64;
				uint32 v32[2];
			} s, d;
			s.v64 = u;
			d.v32[0] = Swap32(s.v32[1]);
			d.v32[1] = Swap32(s.v32[0]);
			return d.v64;
#endif
		}

	}

}

#endif

