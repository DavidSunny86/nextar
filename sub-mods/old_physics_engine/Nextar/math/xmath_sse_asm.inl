
//@ Abhishek Dey
//@ June 27,2006
//@ September 04 2007
//@ October 24 2007 (Ooha Its been a long time since I started SIMD!!:D)
//@ Nov 04 2007
//===========================================================================
// This file contains SSE specific ASM defn
// The compliant intrinsic version was ripped off.
// I might include it later. (That was a whole lot of code after all)
// All functions in this file are tested
// All codes in Assembly were written by Abhishek Dey (i.e. me), so if some code is
// not comprehensive, its because I invented them and there is no well defined algo for these codes...
// + these codes are guranteed to work at least in x86 architecture. For IA64, sse should be disabled.
#ifdef _MSC_VER
#	pragma warning( disable : 4035 )
#endif
// IMPORTANT NOTE::
// This file contains asm version of the inline functions
#include <xmmintrin.h>

#ifdef _m128c
#undef _m128c
#endif

#define _m128c(ptr) (*(__m128*)(ptr))
#define xM12CpyMac(m1,m2) \
	_m128c(m1+0) = _mm_load_ps((const f32*)(m2+0)); \
	_m128c(m1+4) = _mm_load_ps((const f32*)(m2+4)); \
	_m128c(m1+8) = _mm_load_ps((const f32*)(m2+8)); //SAFE_PTR_USAGE
#define xM16CpyMac(m1,m2) \
	_m128c(m1+0) = _mm_load_ps((const f32*)(m2+0)); \
	_m128c(m1+4) = _mm_load_ps((const f32*)(m2+4)); \
	_m128c(m1+8) = _mm_load_ps((const f32*)(m2+8)); \
	_m128c(m1+12) = _mm_load_ps((const f32*)(m2+12)); //SAFE_PTR_USAGE
#define xQ4CpyMac(q1,q2) _m128c(q1) = _mm_load_ps((const f32*)(q2));
//===========================================================================

//===========================================================================
// cpy 1
_n_inline void xM12Cpy(f32* m1,const f32* m2)
{
	__asm
	{
		mov		eax, m1
		mov		ecx, m2
		movaps	xmm0,[ecx]
		movaps	[eax],xmm0
		movaps	xmm0,[ecx+16]
		movaps	[eax+16],xmm0
		movaps	xmm0,[ecx+32]
		movaps	[eax+32],xmm0
	}
}
// cpy 2
_n_inline void xM16Cpy(f32* m1,const f32* m2)
{
	__asm
	{
		mov		eax, m1
		mov		ecx, m2
		movaps	xmm0,[ecx]
		movaps	[eax],xmm0
		movaps	xmm0,[ecx+16]
		movaps	[eax+16],xmm0
		movaps	xmm0,[ecx+32]
		movaps	[eax+32],xmm0
		movaps	xmm0,[ecx+48]
		movaps	[eax+48],xmm0
	}
}
// cpy 3
_n_inline void xQ4Cpy(f32* q1,const f32* q2)
{
	__asm
	{
		mov		eax, q2
		movaps	xmm0,[eax]
		mov		eax, q1
		movaps	[eax],xmm0
	}
}
//===========================================================================
// quads
//===========================================================================
// vector length
_n_inline void xQ3LenSq(f32* store,const f32* v)
{
	__asm
	{
		mov		eax, v
		movaps	xmm0, [eax]
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,0FDh
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		mov		eax, store
		movss	[eax],xmm1
	}
}
// vector length
_n_inline f32 xQ3LenSqR(const f32* v)
{
	register f32 retval;
	__asm
	{
		mov		eax, v
		movaps	xmm0, [eax]
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,0FDh
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		movss	retval,xmm1
	}
	return retval;
}
// vector length
_n_inline void xQ3Len(f32* store,const f32* v)
{
	__asm
	{
		mov		eax, v
		movaps	xmm0, [eax]
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,0FDh
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		sqrtss	xmm1,xmm1
		mov		eax, store
		movss	[eax],xmm1
	}
}
// vector length
_n_inline f32 xQ3LenR(const f32* v)
{
	register f32 retval;
	__asm
	{
		mov		eax, v
		movaps	xmm0, [eax]
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,0FDh
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		sqrtss	xmm1,xmm1
		movss	retval,xmm1
	}
	return retval;
}

// 4 val len
_n_inline void xQ4LenSq(f32* store,const f32* v)
{
	__asm
	{
		mov		eax,v
		movaps	xmm0,[eax]
		mov		eax,store
		movaps	xmm2,xmm0
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm1,xmm0
		movss	[eax],xmm1
	}
}
// vector length
_n_inline f32 xQ4LenSqR(const f32* v)
{
	register f32 retval;
	__asm
	{
		mov		eax,v
		movaps	xmm0,[eax]
		movaps	xmm2,xmm0
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm1,xmm0
		movss	retval,xmm1
	}	
	return retval;
}
// vector length
_n_inline void xQ4Len(f32* store,const f32* v)
{
	__asm
	{
		mov		eax,v
		movaps	xmm0,[eax]
		mov		eax,store
		movaps	xmm2,xmm0
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm1,xmm0
		sqrtss	xmm1,xmm1
		movss	[eax],xmm1
	}
}
// vector length
_n_inline f32 xQ4LenR(const f32* v)
{
	register f32 retval;
	__asm
	{
		mov		eax,v
		movaps	xmm0,[eax]
		movaps	xmm2,xmm0
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm1,xmm0
		sqrtss	xmm1,xmm1
		movss	retval,xmm1
	}
	return retval;
}
// vector v normalize
_n_inline void xQ3Normalize(f32* v) 
{
	__asm
	{
		mov		eax, v
		movaps	xmm0,[eax]
		movaps	xmm2,xmm0
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,0FDh
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		sqrtss	xmm1,xmm1
		shufps	xmm1,xmm1,0
		divps	xmm2,xmm1
		movaps	[eax],xmm2
	}
}
// vector v normalize
_n_inline void xQ4Normalize(f32* v) 
{
	__asm
	{
		mov		eax, v
		movaps	xmm0,[eax]
		movaps	xmm2,xmm0
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm1,xmm0
		sqrtss	xmm1,xmm1
		shufps	xmm1,xmm1,0
		divps	xmm2,xmm1
		movaps	[eax],xmm2
	}
}

//TESTED
_n_inline void xPNormalize(f32* q)
{
	__asm
	{
		mov		eax, q
		movaps	xmm0, [eax]
		movaps	xmm2,xmm0
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		sqrtss	xmm0,xmm0
		shufps	xmm0,xmm0,0
		divps	xmm2,xmm0
		movaps	[eax],xmm2
	}
}

//TESTED
_n_inline void xPNormalizeS(f32* q1,const f32* q)
{
	__asm
	{
		mov		eax, q
		movaps	xmm0, [eax]
		movaps	xmm2,xmm0
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		sqrtss	xmm0,xmm0
		shufps	xmm0,xmm0,0
		divps	xmm2,xmm0
		mov		eax, q1
		movaps	[eax],xmm2
	}
}

// vector v normalize
_n_inline void xQ3NormalizeS(f32* q1,const f32* q2)
{
	__asm
	{
		mov		eax, q2
		movaps	xmm0,[eax]
		mov		eax, q1
		movaps	xmm2,xmm0
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,0FDh
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		sqrtss	xmm1,xmm1
		shufps	xmm1,xmm1,0
		divps	xmm2,xmm1
		movaps	[eax],xmm2
	}
}
// vector v normalize
_n_inline void xQ4NormalizeS(f32* q1,const f32* q2)
{
	__asm
	{
		mov		eax, q2
		movaps	xmm0,[eax]
		mov		eax, q1
		movaps	xmm2,xmm0
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,2,1)
		addss	xmm1,xmm0
		sqrtss	xmm1,xmm1
		shufps	xmm1,xmm1,0
		divps	xmm2,xmm1
		movaps	[eax],xmm2
	}
}
// scale
_n_inline void xQ4ScaleS(f32* store,const f32* v,f32 val)
{
	__asm
	{
		movss	xmm1,val
		mov		eax, v
		movaps	xmm0,[eax]
		shufps  xmm1,xmm1,0h
		mulps	xmm0,xmm1
		mov		eax,[store]
		movaps	[eax],xmm0
	}
}
// scale
_n_inline void xQ3ScaleS(f32* store,const f32* v,f32 val)
{
	__asm
	{
		movss	xmm1,val
		mov		eax, v
		movaps	xmm0,[eax]
		shufps  xmm1,xmm1,40h
		mulps	xmm0,xmm1
		mov		eax,[store]
		movaps	[eax],xmm0
	}
}
// scale
_n_inline void xQ4Scale(f32* v,f32 val)
{
	__asm
	{
		movss	xmm0,val
		shufps  xmm0,xmm0,0
		mov		eax, v
		mulps	xmm0,[eax]
		movaps	[eax],xmm0
	}
}
// scale
_n_inline void xQ3Scale(f32* v,f32 val)
{
	__asm
	{
		movss	xmm1,val
		mov		eax, v
		movaps	xmm0,[eax]
		shufps  xmm1,xmm1,40h
		mulps	xmm0,xmm1
		movaps	[eax],xmm0
	}
}
// i.e store += scale*vec v3,v4 ans:v3
_n_inline void xQ3ScaleNAdd(f32* store,const f32* q,f32 val)
{
	__asm 
	{
		movss	xmm0,val
		shufps	xmm0,xmm0,40h
		mov		eax,[q]
		movaps	xmm1,[eax]
		mulps	xmm0,xmm1
		mov		eax,[store]
		addps	xmm0,[eax]
		movaps	[eax],xmm0
	}
}
// i.e store += scale*vec v3,v4 ans:v3
_n_inline void xQ4ScaleNAdd(f32* store,const f32* q,f32 val)
{
	__asm 
	{
		movss	xmm0,val
		shufps	xmm0,xmm0,0h
		mov		eax,[q]
		movaps	xmm1,[eax]
		mulps	xmm0,xmm1
		mov		eax,[store]
		addps	xmm0,[eax]
		movaps	[eax],xmm0
	}
}
// i.e store -= scale*vec v3,v4 ans:v3
_n_inline void xQ3ScaleNSub(f32* store,const f32* q,f32 val)
{
	__asm 
	{
		movss	xmm0,val
		shufps	xmm0,xmm0,40h
		mov		eax,[q]
		movaps	xmm1,[eax]
		mulps	xmm0,xmm1
		mov		eax,[store]
		movaps	xmm1,[eax]
		subps	xmm1,xmm0
		movaps	[eax],xmm1
	}
}
// i.e store -= scale*vec v3,v4 ans:v3
_n_inline void xQ4ScaleNSub(f32* store,const f32* q,f32 val)
{
	__asm 
	{
		movss	xmm0,val
		shufps	xmm0,xmm0,0h
		mov		eax,[q]
		movaps	xmm1,[eax]
		mulps	xmm0,xmm1
		mov		eax,[store]
		movaps	xmm1,[eax]
		subps	xmm1,xmm0
		movaps	[eax],xmm1
	}
}
// i.e store *= scale*vec v3,v4 ans:v3
_n_inline void xQ3ScaleNMul(f32* store,const f32* q,f32 val) //SAFE_PTR_USAGE
{
	__asm 
	{
		movss	xmm0,val
		shufps	xmm0,xmm0,40h
		mov		eax,[q]
		movaps	xmm1,[eax]
		mulps	xmm0,xmm1
		mov		eax,[store]
		movaps	xmm1,[eax]
		mulps	xmm1,xmm0
		movaps	[eax],xmm1
	}
}

_n_inline void xQ4ScaleNMul(f32* store,const f32* q,f32 val) //SAFE_PTR_USAGE
{
	__asm 
	{
		movss	xmm0,val
		shufps	xmm0,xmm0,0h
		mov		eax,[q]
		movaps	xmm1,[eax]
		mulps	xmm0,xmm1
		mov		eax,[store]
		movaps	xmm1,[eax]
		mulps	xmm1,xmm0
		movaps	[eax],xmm1
	}
}
// i.e store /= scale*vec v3,v4 ans:v3
_n_inline void xQ3ScaleNDiv(f32* store,const f32* q,f32 val) //SAFE_PTR_USAGE
{
	__asm 
	{
		movss	xmm0,val
		shufps	xmm0,xmm0,40h
		mov		eax,[q]
		movaps	xmm1,[eax]
		mulps	xmm0,xmm1
		mov		eax,[store]
		movaps	xmm1,[eax]
		divps	xmm1,xmm0
		movaps	[eax],xmm1
	}
}

_n_inline void xQ4ScaleNDiv(f32* store,const f32* q,f32 val) //SAFE_PTR_USAGE
{
	__asm 
	{
		movss	xmm0,val
		shufps	xmm0,xmm0,0h
		mov		eax,[q]
		movaps	xmm1,[eax]
		mulps	xmm0,xmm1
		mov		eax,[store]
		movaps	xmm1,[eax]
		divps	xmm1,xmm0
		movaps	[eax],xmm1
	}
}
// xQ4Add
_n_inline void xQ4Add(f32* store,const f32* v1,const f32* v2)
{
	__asm 
	{
		mov		eax,[v1]
		movaps	xmm0,[eax]
		mov		eax,[v2]
		addps	xmm0,[eax]
		mov		eax,[store]
		movaps	[eax],xmm0
	}
}
// xQ4Sub
_n_inline void xQ4Sub(f32* store,const f32* v1,const f32* v2)
{
	__asm 
	{
		mov		eax,[v1]
		movaps	xmm0,[eax]
		mov		eax,[v2]
		subps	xmm0,[eax]
		mov		eax,[store]
		movaps	[eax],xmm0
	}
}
// xQ4Mul
_n_inline void xQ4Mul(f32* store,const f32* v1,const f32* v2)
{
	__asm 
	{
		mov		eax,[v1]
		movaps	xmm0,[eax]
		mov		eax,[v2]
		mulps	xmm0,[eax]
		mov		eax,[store]
		movaps	[eax],xmm0
	}
}
// xQ4Div
_n_inline void xQ4Div(f32* store,const f32* v1,const f32* v2)
{
	__asm 
	{
		mov		eax,[v1]
		movaps	xmm0,[eax]
		mov		eax,[v2]
		divps	xmm0,[eax]
		mov		eax,[store]
		movaps	[eax],xmm0
	}
}

// Interpolate
_n_inline void xQ4Interpolate(f32* store,const f32* v2,f32 factor)
{
	__asm 
	{
		movss	xmm0,factor		; load factor
		shufps	xmm0,xmm0,0		; into xmm0
		mov		eax,[v2]		; load v2 into
		movaps	xmm1,[eax]		; xmm1
		mov		eax,[store]		; substract store
		subps	xmm1,[eax]		; from xmm1
		mulps	xmm1,xmm0		; scale xmm1 by xmm0
		addps	xmm1,[eax]		; add store to xmm1
		movaps	[eax],xmm1		; store xmm1
	}
}

// xVCross: v3,v4 (both parms) ans:v3
_n_inline void xVCross(f32* store,const f32* v1,const f32* v2) 
{
	__asm
	{
		mov eax, v1
		movaps xmm0, [eax]
		movaps xmm1,xmm0
		shufps xmm0,xmm0,0C9h
		mov eax, v2
		movaps xmm2, [eax]
		mulps xmm0,xmm2
		shufps xmm2,xmm2,0C9h
		mulps xmm2,xmm1
		subps xmm2,xmm0
		shufps xmm2,xmm2,0C9h
		mov eax,store
		movaps [eax],xmm2 
	}
}

// Dot Function for 4 element aligned vector
// use the asm version for most advantage
_n_inline void  xVDot(f32* store,const f32* v1,const f32* v2) 
{
	__asm
	{
		mov		eax, v1
		movaps  xmm0, xmmword ptr[eax]
		mov		eax, v2
		mulps	xmm0, xmmword ptr[eax]  ;xmm0 = v1 * v2
		movaps  xmm1,xmm0
		shufps	xmm1,xmm0,0FDh			;xmm1 = shuffle(xmm0,xmm0)
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		mov		eax, store
		movss	dword ptr[eax], xmm1
	}
}
// Dot Function for 4 element aligned vector
// use the asm version for most advantage
_n_inline f32  xVDotR(const f32* v1,const f32* v2) 
{
	register f32 retval;
	__asm
	{
		mov		eax, v1
		movaps  xmm0, xmmword ptr[eax]
		mov		eax, v2
		mulps	xmm0, xmmword ptr[eax]  ;xmm0 = v1 * v2
		movaps  xmm1,xmm0
		shufps	xmm1,xmm0,0FDh			;xmm1 = shuffle(xmm0,xmm0)
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		movss	retval, xmm1
	}
	return retval;
}
// vector distance
_n_inline void xVDistance(f32* store,const f32* v1,const f32* v2)
{
	__asm 
	{
		mov		eax, v1
		movaps	xmm0, [eax]
		mov		eax, v2
		subps	xmm0,[eax]
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,0FDh
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		sqrtss	xmm1,xmm1
		mov		eax, store
		movss	[eax],xmm1
	}
}
// vector distance
_n_inline f32 xVDistanceR(const f32* v1,const f32* v2)
{
	register f32 store;
	__asm 
	{
		mov		eax, v1
		movaps	xmm0, [eax]
		mov		eax, v2
		subps	xmm0,[eax]
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,0FDh
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		sqrtss	xmm1,xmm1
		movss	store, xmm1
	}
	return store;
}
// vector distance square
_n_inline void xVDistanceSq(f32* store,const f32* v1,const f32* v2)
{
	__asm 
	{
		mov		eax, v1
		movaps	xmm0, [eax]
		mov		eax, v2
		subps	xmm0,[eax]
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,0FDh
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		mov		eax, store
		movss	[eax],xmm1
	}
}
// vector distance square
_n_inline f32 xVDistanceSqR(const f32* v1,const f32* v2)
{
	register f32 store;
	__asm 
	{
		mov		eax, v1
		movaps	xmm0, [eax]
		mov		eax, v2
		subps	xmm0,[eax]
		mulps	xmm0,xmm0
		movaps	xmm1,xmm0
		shufps	xmm1,xmm1,0FDh
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		movss	store, xmm1
	}
	return store;
}

// TESTED
// dot each row of r with a and store
_n_inline void xMul0_344(f32* store,const f32* a,const f32* r)
{
	__asm 
	{
		mov		eax, a
		movaps	xmm0,[eax]
		mov		eax, r
		movaps	xmm1,xmm0
		mulps	xmm1,[eax]
		movaps	xmm2,xmm0
		mulps	xmm2,[eax+16]
		mulps	xmm0,[eax+32]
		mov		eax, [store]

		movaps	xmm3,xmm1
		shufps	xmm3,xmm1,0FDh
		addss	xmm3,xmm1
		shufps	xmm1,xmm1,0FEh
		addss	xmm3,xmm1
		movss	[eax],xmm3

		movaps	xmm3,xmm2
		shufps	xmm3,xmm2,0FDh
		addss	xmm3,xmm2
		shufps	xmm2,xmm2,0FEh
		addss	xmm3,xmm2
		movss	[eax+4],xmm3

		movaps	xmm3,xmm0
		shufps	xmm3,xmm0,0FDh
		addss	xmm3,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm3,xmm0
		movss	[eax+8],xmm3
	}
}

// TESTED
// store = a*r. Where r is a matrices 3x4 and a is a vector multiplications of the form A*R(row(i)*coloumn(i))
_n_inline void xMul1_344(f32* store,const f32* a,const f32* r)
{
	__asm
	{
		mov		eax, [r]
						
		movaps	xmm1,[eax]
		movaps	xmm2,[eax+16]
		movaps	xmm3,[eax+32]
			
		mov		eax, [a]

		movss	xmm0,[eax]
		shufps	xmm0,xmm0,40h
		mulps	xmm1,xmm0

		movss	xmm0,[eax+4]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm1,xmm0

		movss	xmm0,[eax+8]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm1,xmm0

		mov		eax, [store]
		movaps	[eax],xmm1
	}
}

// multiply each row of r with a, add each COLOUMN of r and store in store
_n_inline void xMul2_344(f32* store,const f32* a,const f32* r)
{
	__asm 
	{
		mov		eax, r
		movaps	xmm0,[eax]
		addps	xmm0,[eax+16]
		addps	xmm0,[eax+32]
		mov		eax, a
		mulps	xmm0,[eax]
		mov		eax, store
		movaps	[eax],xmm0
	}
}

// dot 
_n_inline void xMul0_34(f32* store,const f32* a,const f32* b)
{
	// using loop version
	__asm
	{
		mov		ecx, a
		mov		ebx, store
		mov		eax, b
		movaps	xmm1,[eax+00]
		movaps	xmm2,[eax+16]
		movaps	xmm3,[eax+32]
	
		mov		eax, 0

loop_mul034:

		movaps	xmm0,xmmword ptr[ecx+eax]

		movaps	xmm4,xmm1
		mulps	xmm4,xmm0

		movaps	xmm5,xmm4
		shufps	xmm5,xmm4,0FDh
		addss	xmm5,xmm4
		shufps	xmm4,xmm4,0FEh
		addss	xmm5,xmm4
		movss	[ebx+eax],xmm5

		movaps	xmm4,xmm2
		mulps	xmm4,xmm0

		movaps	xmm5,xmm4
		shufps	xmm5,xmm4,0FDh
		addss	xmm5,xmm4
		shufps	xmm4,xmm4,0FEh
		addss	xmm5,xmm4
		movss	[ebx+eax+4],xmm5

		movaps	xmm4,xmm3
		mulps	xmm4,xmm0

		movaps	xmm5,xmm4
		shufps	xmm5,xmm4,0FDh
		addss	xmm5,xmm4
		shufps	xmm4,xmm4,0FEh
		addss	xmm5,xmm4
		movss	[ebx+eax+8],xmm5

		add		eax,16
		cmp		eax,48
		jne		loop_mul034
	}
}

// store = a*r
// where a 'n r are matrices 4x3
// multiplications of the form A*R(row(i)*coloumn(i))
// to suite simd we multiply it in a non conventional way
// i created this code snippet after becoming really confused
// about how to implement simd with multiplication.
// this idea can be extended to any n*4 matrix efficienly 
// generating a single row at each iteration
// the concept is to select an element from the first matrix
// and multiply it to the rth row of the matrix b and add it
// to a sum variable which tracks each iteration's result.
// we do not loop cause not many instructions are needed
// and we shud avoid a jump
_n_inline void xMul1_34(f32* store, const f32* a,const f32* r)
{
	__asm
	{
		mov		eax, [a]
		mov		ecx, [r]
				
		movaps	xmm1,[ecx]
		movaps	xmm2,[ecx+16]
		movaps	xmm3,[ecx+32]
		mov		ecx, [store]

		movss	xmm0,[eax]
		shufps	xmm0,xmm0,40h
		movaps	xmm4,xmm0
		mulps	xmm4,xmm1

		movss	xmm0,[eax+4]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm4,xmm0

		movss	xmm0,[eax+8]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm4,xmm0

		movaps	[ecx],xmm4

		movss	xmm0,[eax+16]
		shufps	xmm0,xmm0,40h
		movaps	xmm4,xmm0
		mulps	xmm4,xmm1

		movss	xmm0,[eax+20]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm4,xmm0

		movss	xmm0,[eax+24]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm4,xmm0

		movaps	[ecx+16],xmm4

		movss	xmm0,[eax+32]
		shufps	xmm0,xmm0,40h
		movaps	xmm4,xmm0
		mulps	xmm4,xmm1

		movss	xmm0,[eax+36]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm4,xmm0

		movss	xmm0,[eax+40]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm4,xmm0

		movaps	[ecx+32],xmm4
	}
}

// TESTED
// i.e store = a'*b. form < s(i,j) = col(a,i). col(b,j) >
//===========================================================================
_n_inline void xMul2_34(f32* store, const f32* a,const f32* b)
{
	__asm
	{
		mov		eax, [a]
		mov		ecx, [b]
				
		movaps	xmm1,[ecx]
		movaps	xmm2,[ecx+16]
		movaps	xmm3,[ecx+32]
		mov		ecx, [store]

		movss	xmm0,[eax]
		shufps	xmm0,xmm0,40h
		movaps	xmm4,xmm0
		mulps	xmm4,xmm1

		movss	xmm0,[eax+16]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm4,xmm0

		movss	xmm0,[eax+32]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm4,xmm0

		movaps	[ecx],xmm4

		movss	xmm0,[eax+4]
		shufps	xmm0,xmm0,40h
		movaps	xmm4,xmm0
		mulps	xmm4,xmm1

		movss	xmm0,[eax+20]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm4,xmm0

		movss	xmm0,[eax+36]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm4,xmm0

		movaps	[ecx+16],xmm4

		movss	xmm0,[eax+8]
		shufps	xmm0,xmm0,40h
		movaps	xmm4,xmm0
		mulps	xmm4,xmm1

		movss	xmm0,[eax+24]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm4,xmm0

		movss	xmm0,[eax+40]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm4,xmm0

		movaps	[ecx+32],xmm4
	}
}

// simple matrix transpose
_n_inline void xMTrans_34(f32* store_m,const f32* in_m)
{
	__asm
	{
		mov	   eax, in_m
		movaps xmm0, [eax]
		movaps xmm1, [eax+16]
		movaps xmm2, [eax+32]
		mov	   eax, store_m
		movaps xmm3,xmm1
		shufps xmm3,xmm2,_MM_SHUFFLE(3,0,0,3)
		movss  xmm3,xmm0
		movaps [eax],xmm3
		movaps xmm3,xmm0
		shufps xmm3,xmm1,_MM_SHUFFLE(3,1,3,1)
		shufps xmm3,xmm2,_MM_SHUFFLE(3,1,2,0)
		movaps [eax+16],xmm3
		movaps xmm3,xmm0
		shufps xmm3,xmm1,_MM_SHUFFLE(3,2,3,2)
		shufps xmm3,xmm2,_MM_SHUFFLE(3,2,2,0)
		movaps [eax+32],xmm3
	}
}

// simple matrix transpose
// NOT TESTED, Seems alright though as its only _MM_TRANSPOSE's asm version
_n_inline void xMTrans_44(f32* store_m,const f32* in_m)
{
	__asm
	{   
		mov	   eax, in_m
		movaps xmm0, [eax]
		movaps xmm1, [eax+16]
		movaps xmm3, [eax+32]
		movaps xmm4, [eax+48]
		mov	   eax, store_m
		movaps xmm2,xmm0
		shufps xmm0,xmm1,0x44
		shufps xmm2,xmm1,0xEE
		movaps xmm1,xmm3
		shufps xmm1,xmm4,0x44
		shufps xmm3,xmm4,0xEE
		movaps xmm4,xmm0
		shufps xmm0,xmm1,0x88
		movaps [eax],xmm0
		shufps xmm4,xmm1,0xDD
		movaps [eax+16],xmm4
		movaps xmm4,xmm2
		shufps xmm2,xmm3,0x88
		movaps [eax+32],xmm2
		shufps xmm4,xmm3,0xDD
		movaps [eax+48],xmm4
	}
}

// 4x4 matrix multiplication
_n_inline void xMul1_44(f32* store, const f32* a,const f32* b)
{
	__asm
	{
		mov		ebx, [a]
		mov		ecx, [b]
				
		movaps	xmm1,[ecx]
		movaps	xmm2,[ecx+16]
		movaps	xmm3,[ecx+32]
		movaps	xmm4,[ecx+48]

		mov		ecx, store
		mov		eax, 0

loop_mul144:
		movss	xmm0,[ebx+eax]
		shufps	xmm0,xmm0,0h
		movaps	xmm5,xmm0
		mulps	xmm5,xmm1

		movss	xmm0,[ebx+eax+4]
		shufps	xmm0,xmm0,0h
		mulps	xmm0,xmm2
		addps	xmm5,xmm0

		movss	xmm0,[ebx+eax+8]
		shufps	xmm0,xmm0,0h
		mulps	xmm0,xmm3
		addps	xmm5,xmm0

		movss	xmm0,[ebx+eax+12]
		shufps	xmm0,xmm0,0h
		mulps	xmm0,xmm4
		addps	xmm5,xmm0

		movaps	[ecx+eax],xmm5

		add		eax,16
		cmp		eax,64
		jne		loop_mul144
	}
}

_n_inline void xMul1_44o(f32* store,const f32* a,const f32* b)
{
	__asm
	{
		mov		ebx, [a]
		mov		ecx, [b]
				
		movaps	xmm1,[ecx]
		movaps	xmm2,[ecx+16]
		movaps	xmm3,[ecx+32]
		movaps	xmm4,[ecx+48]

		mov		ecx, store
		mov		eax, 0
loop_mul144o:	
		movss	xmm0,[ebx+eax]
		shufps	xmm0,xmm0,40h
		movaps	xmm5,xmm0
		mulps	xmm5,xmm1

		movss	xmm0,[ebx+eax+4]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm5,xmm0

		movss	xmm0,[ebx+eax+8]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm5,xmm0

		movaps	[ecx+eax],xmm5	; avoiding one jump, we could send this code before loop_mul144o and start

		add		eax,16
		cmp		eax,64
		jne		loop_mul144o

		addps	xmm5,xmm4
		movaps	[ecx+eax-16],xmm5
	}
}

_n_inline void xMul1_444(f32* store,const f32* a,const f32* r)
{
	__asm
	{
		mov		eax, [r]
					
		movaps	xmm1,[eax]
		movaps	xmm2,[eax+16]
		movaps	xmm3,[eax+32]
		movaps	xmm4,[eax+48]
		
		mov		eax, [a]

		movss	xmm0,[eax]
		shufps	xmm0,xmm0,40h
		mulps	xmm1,xmm0

		movss	xmm0,[eax+4]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm1,xmm0

		movss	xmm0,[eax+8]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm1,xmm0

		movss	xmm0,[eax+12]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm4
		addps	xmm1,xmm0

		mov		eax, [store]
		movaps	[eax],xmm1
	}
}

// optimized
_n_inline void xMul1_444o(f32* store,const f32* a,const float* r)
{
	__asm
	{
		mov		eax, [r]
					
		movaps	xmm1,[eax]
		movaps	xmm2,[eax+16]
		movaps	xmm3,[eax+32]
		movaps	xmm4,[eax+48]
		
		mov		eax, [a]

		movss	xmm0,[eax]
		shufps	xmm0,xmm0,40h
		mulps	xmm1,xmm0

		movss	xmm0,[eax+4]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm1,xmm0

		movss	xmm0,[eax+8]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm1,xmm0
		addps	xmm1,xmm4

		mov		eax, [store]
		movaps	[eax],xmm1
	}
}

_n_inline void xMul1_443(f32* store,const f32* a,const f32* r)
{
	__asm
	{
		mov		eax, [r]
					
		movaps	xmm1,[eax]
		movaps	xmm2,[eax+16]
		movaps	xmm3,[eax+32]
		movaps	xmm4,[eax+48]
		
		mov		eax, [a]

		movss	xmm0,[eax]
		shufps	xmm0,xmm0,40h
		mulps	xmm1,xmm0

		movss	xmm0,[eax+4]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm1,xmm0

		movss	xmm0,[eax+8]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm1,xmm0
		addps	xmm1,xmm4

		mov		eax, [store]
		movss	[eax],xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(0,0,2,1)
		movss	[eax+4],xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(0,0,0,1)
		movss	[eax+8],xmm1
	}
}

// simple plane_t construction
_n_inline void xMakePlane(f32* store_ptr,const f32* p1,const f32* p2,const f32* p3)
{
	__asm
	{
		mov eax, p1
		movaps xmm0, [eax]
		mov eax, p2
		movaps xmm1, [eax]
		mov eax, p3
		movaps xmm2, [eax]
		movaps xmm3, xmm1
		mov eax, store_ptr

		subps  xmm3, xmm0
		subps  xmm2, xmm1
	
		movaps xmm1,xmm3
		shufps xmm3,xmm3,0C9h
		mulps  xmm3,xmm2
		shufps xmm2,xmm2,0C9h
		mulps  xmm2,xmm1
		subps  xmm2,xmm3
		shufps xmm2,xmm2,0C9h

		movaps	xmm1,xmm2
		mulps	xmm2,xmm2
		movaps	xmm3,xmm2
		shufps	xmm3,xmm3,0FDh
		addss	xmm3,xmm2
		shufps	xmm2,xmm2,0FEh
		addss	xmm3,xmm2
		sqrtss	xmm3,xmm3
		shufps	xmm3,xmm3,0
		divps	xmm1,xmm3
		movaps	[eax],xmm1

		mulps	xmm0,xmm1				;xmm0 = v1 * v2
		movaps  xmm1,xmm0
		shufps	xmm1,xmm0,0FDh			;xmm1 = shuffle(xmm0,xmm0)
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		movss	[eax+12], xmm1
		fld		[eax+12]
		fchs
		fstp	[eax+12]
	}
}

// n count quads
_n_inline void xMul1_444o_n(vec4_t* store,const vec4_t* a,const f32* r,s32 cnt)
{
	__asm
	{
		mov		ecx, cnt
		mov		eax, r
		movaps	xmm1,[eax]
		movaps	xmm2,[eax+16]
		movaps	xmm3,[eax+32]
		movaps	xmm4,[eax+48]
		
		mov		eax, a
		mov		ebx, store
	
loop_xmul1_444o_n:

		movss	xmm0,[eax]
		shufps	xmm0,xmm0,40h
		mulps	xmm1,xmm0

		movss	xmm0,[eax+4]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm1,xmm0

		movss	xmm0,[eax+8]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm1,xmm0
		addps	xmm1,xmm4

		movaps	[ebx],xmm1
		add		ebx, 16
		add		eax, 16
		loop	loop_xmul1_444o_n
	}
}

// n count vector
_n_inline void xMul1_443o_n(vec3_t* store,const vec3_t* a,const f32* r,s32 cnt)
{
	__asm
	{
		mov		ecx, cnt
		mov		eax, r
		movaps	xmm1,[eax]
		movaps	xmm2,[eax+16]
		movaps	xmm3,[eax+32]
		movaps	xmm4,[eax+48]

		mov		eax, a
		mov		ebx, store

loop_xmul1_443o_n:

		movss	xmm0,[eax]
		shufps	xmm0,xmm0,40h
		mulps	xmm1,xmm0

		movss	xmm0,[eax+4]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm1,xmm0

		movss	xmm0,[eax+8]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm1,xmm0
		addps	xmm1,xmm4

		movss	[ebx],xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(0,0,2,1)
		movss	[ebx+4],xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(0,0,0,1)
		movss	[ebx+8],xmm1
		add		ebx, 16
		add		eax, 16
		loop	loop_xmul1_443o_n
	}
}

// n count quads
_n_inline void xMul1_344o_n(f32* store,const f32* a,const f32* r,s32 cnt)
{
	__asm
	{
		mov		ecx, cnt
		mov		eax, r
						
		movaps	xmm1,[eax]
		movaps	xmm2,[eax+16]
		movaps	xmm3,[eax+32]
			
		mov		eax, a
		mov		ebx, store

loop_xmul1_344o_n:

		movss	xmm0,[eax]
		shufps	xmm0,xmm0,40h
		mulps	xmm1,xmm0

		movss	xmm0,[eax+4]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm1,xmm0

		movss	xmm0,[eax+8]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm1,xmm0

		movaps	[ebx],xmm1
		add		ebx, 16
		add		eax, 16
		loop	loop_xmul1_344o_n

	}
}

// NOT TESTED 
_n_inline void xPFromPoints(f32* store_ptr,const f32* p1,const f32* p2,const f32* p3)
{
	__asm
	{
		mov eax, p1
		movaps xmm0, [eax]
		mov eax, p2
		movaps xmm1, [eax]
		mov eax, p3
		movaps xmm2, [eax]
		mov eax, store_ptr

		movaps xmm3, xmm1

		subps  xmm3, xmm0
		subps  xmm2, xmm1
	
		movaps xmm1,xmm3
		shufps xmm3,xmm3,0C9h
		mulps  xmm3,xmm2
		shufps xmm2,xmm2,0C9h
		mulps  xmm2,xmm1
		subps  xmm2,xmm3
		shufps xmm2,xmm2,0C9h

		movaps	xmm1,xmm2
		mulps	xmm2,xmm2
		movaps	xmm3,xmm2
		shufps	xmm3,xmm3,0FDh
		addss	xmm3,xmm2
		shufps	xmm2,xmm2,0FEh
		addss	xmm3,xmm2
		sqrtss	xmm3,xmm3
		shufps	xmm3,xmm3,0
		divps	xmm1,xmm3
		movaps	[eax],xmm1

		mulps	xmm0,xmm1				;xmm0 = v1 * v2
		movaps  xmm1,xmm0
		shufps	xmm1,xmm0,0FDh			;xmm1 = shuffle(xmm0,xmm0)
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		movss	[eax+12], xmm1
		fld		[eax+12]
		fchs
		fstp	[eax+12]
	}
}

// TESTED 
_n_inline f32 xPCalculatePointR(const f32* pl,const f32* point)
{
	register f32 retval;
	__asm
	{
		mov		eax, pl
		movaps  xmm0,xmmword ptr[eax]
		movaps	xmm1,xmm0
		mov		eax, point
		mulps	xmm0,[eax]
		shufps	xmm1,xmm0,_MM_SHUFFLE(2,1,3,3)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,3,2)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,3,3)
		addss	xmm0,xmm1
		movss	retval,xmm0
	}
	return retval;
}

// TESTED 
_n_inline void xPCalculatePoint(f32* store_ptr,const f32* pl,const f32* point)
{
	__asm
	{
		mov		eax, pl
		movaps  xmm0,xmmword ptr[eax]
		movaps	xmm1,xmm0
		mov		eax, point
		mulps	xmm0,[eax]
		shufps	xmm1,xmm0,_MM_SHUFFLE(2,1,3,3)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,3,2)
		addss	xmm0,xmm1
		shufps	xmm1,xmm1,_MM_SHUFFLE(3,3,3,3)
		addss	xmm0,xmm1
		mov		eax,store_ptr
		movss	[eax],xmm0
	}
}

_n_inline void xTMulEfficient(transform* store,const transform* off,const transform* parent)
{
	__asm
	{
		
		mov		eax, [parent]
						
		movaps	xmm1,[eax]
		movaps	xmm2,[eax+16]
		movaps	xmm3,[eax+32]
		movaps  xmm4,[eax+48]	; change
		movaps	xmm5,xmm1		; change
			
		mov		eax, [off]

		; find abs pos

		movss	xmm0,[eax+48]
		shufps	xmm0,xmm0,40h
		mulps	xmm5,xmm0

		movss	xmm0,[eax+52]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm5,xmm0

		movss	xmm0,[eax+56]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm5,xmm0

		mov		ecx, [store]
		addps	xmm5,xmm4		; change
		movaps	[ecx+48],xmm5

		; multiply rot1*rot2
	
		movss	xmm0,[eax]
		shufps	xmm0,xmm0,40h
		movaps	xmm4,xmm0
		mulps	xmm4,xmm1

		movss	xmm0,[eax+4]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm4,xmm0

		movss	xmm0,[eax+8]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm4,xmm0

		movaps	[ecx],xmm4

		movss	xmm0,[eax+16]
		shufps	xmm0,xmm0,40h
		movaps	xmm4,xmm0
		mulps	xmm4,xmm1

		movss	xmm0,[eax+20]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm4,xmm0

		movss	xmm0,[eax+24]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm4,xmm0

		movaps	[ecx+16],xmm4

		movss	xmm0,[eax+32]
		shufps	xmm0,xmm0,40h
		movaps	xmm4,xmm0
		mulps	xmm4,xmm1

		movss	xmm0,[eax+36]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm2
		addps	xmm4,xmm0

		movss	xmm0,[eax+40]
		shufps	xmm0,xmm0,40h
		mulps	xmm0,xmm3
		addps	xmm4,xmm0

		movaps	[ecx+32],xmm4	
	}
}
#ifdef _MSC_VER
#	pragma warning( default : 4035 )
#endif

