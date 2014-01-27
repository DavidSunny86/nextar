//	1/10/2007		AT:		9:19:26 AM
//	Oct 27 07
//      Jan 2011
#ifndef NEXTAR_MEMORY_H__
#define NEXTAR_MEMORY_H__

#include "NexSys.h"

namespace nextar {

	namespace MemUtils {

		_NexInline void CopyDwords(void* dest, const void* src, size_t nb) {
#if (NEX_ASM_LEVEL == 2) && defined( NEX_MSVC )
			_asm mov ecx, nb
			_asm mov edi, dest
			_asm mov esi, src
			_asm rep movsd
#elif (NEX_ASM_LEVEL == 2) && defined( NEX_GCC )
			__asm__ volatile ("rep movsd\n\t"
					:
					: "c" (nb), "D" (dest), "S" (src));
#else
			std::memcpy(dest, src, nb * sizeof(long));
#endif
		}

		_NexInline void CopyWords(void* dest, const void* src, size_t nb) {
#if (NEX_ASM_LEVEL == 2) && defined( NEX_MSVC )
			_asm mov ecx, nb
			_asm mov edi, dest
			_asm mov esi, src
			_asm rep movsw
#elif (NEX_ASM_LEVEL == 2) && defined( NEX_GCC )
			__asm__ volatile ("rep movsw\n\t"
					:
					: "c" (nb), "D" (dest), "S" (src));
#else
			std::memcpy(dest, src, nb * sizeof(short));
#endif
		}

		_NexInline void StoreDword(void* dest, size_t nb, uint32 value) {
#if (NEX_ASM_LEVEL == 2) && defined( NEX_MSVC )
			_asm mov edi, dest
			_asm mov ecx, nb
			_asm mov eax, value
			_asm rep stosd
#elif (NEX_ASM_LEVEL == 2) && defined( NEX_GCC )
			__asm__ volatile ("rep stosl\n\t"
					:
					: "c" (nb), "D" (dest), "a" (value));
#else
			std::fill(reinterpret_cast<uint32*>(dest),
					reinterpret_cast<uint32*>(dest) + nb, value);
#endif
		}
	}
}

#endif //NEXTAR_MEMORY_H__
