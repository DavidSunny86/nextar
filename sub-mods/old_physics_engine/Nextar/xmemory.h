//@	Abhishek Dey
//@	1/10/2007		AT:		9:19:26 AM
//@	Oct 27 07

#ifndef __XMEMORY_H__
#define __XMEMORY_H__

#include "xtypes_c.h"
#ifndef _MSC_VER
#	include <memory.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

_n_inline void copyDwords(void* dest, void* src, u32 nb)
{
	#ifdef _MSC_VER
		_asm mov ecx, nb
		_asm mov edi, dest
		_asm mov esi, src
		_asm rep movsd
	#else
		while(nb--) *((u32*)dest)++ = *((u32*)value)++;
	#endif
}
_n_inline void copyWords(void* dest, void* src, u32 nb)
{
	#ifdef _MSC_VER
		_asm mov ecx, nb
		_asm mov edi, dest
		_asm mov esi, src
		_asm rep movsw
	#else
		while(nb--) *((u16*)dest)++ = *((u16*)value)++;
	#endif
}
_n_inline void storeDwords(void* dest, u32 nb, u32 value)
{
	#ifdef _MSC_VER
		_asm mov edi, dest
		_asm mov ecx, nb
		_asm mov eax, value
		_asm rep stosd
	#else
		while(nb--) *((u32*)dest)++ = value;
	#endif
}
_n_inline void copyMemory(void* dest, void* src, u32 sz)
{
	#ifdef _MSC_VER
		_asm mov ecx, sz
		_asm mov edi, dest
		_asm mov esi, src
		_asm rep movsb
	#else
		memcpy(dest,src,(size_t)sz);
	#endif
}
_n_inline void zeroMemory(void* dest, u32 sz)			
{
	#ifdef _MSC_VER
		_asm mov ecx, sz
		_asm mov edi, dest
		_asm mov eax, 0
		_asm rep stosb
	#else
		memset(addr, 0, sz);
	#endif
	
}
_n_inline void fillMemory(void* dest, u32 sz, byte val)	
{
	#ifdef _MSC_VER
		_asm mov ecx, sz
		_asm mov edi, dest
		_asm mov al, val
		_asm rep stosb
	#else
		memset(addr, val, sz);
	#endif
}

#ifdef __cplusplus
}
#endif

#endif //__XMEMORY_H__