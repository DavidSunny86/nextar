/*FILE MARK
 DOC ORIGINAL NAME:	xMath.inl
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	1/18/2007		AT:		7:20:57 PM
END
******************************************************************************************************
*/
#ifndef XMATH_INL
#define XMATH_INL

#ifdef _MSC_VER
#	pragma warning( disable : 4035 )
#endif

#if !defined(_MSC_VER) && defined(X_USE_ASM_ALTERNATIVES)
#error Deactivate Assembly Alternative macro.(use #undef X_USE_ASM_ALTERNATIVES).
#endif

/* absr */
_n_inline f32 _absr( f32 v ) {
	u32 FloatBits = X_MkInt(v)&0x7fffffff;
	return X_MkFloat(FloatBits);
}

#ifdef X_USE_ASM_ALTERNATIVES
// Fast inverse square root
__inline f32  asmrsqrtr(f32 a) 
{
	__asm 
	{
		mov		eax, 0be6eb508h
		mov		DWORD PTR [esp-12],03fc00000h ;  1.5 on the stack
		sub		eax, DWORD PTR [esp+4]; a
		sub		DWORD PTR [esp+4], 800000h ; a/2 a=Y0
		shr		eax, 1     ; firs approx in eax=R0
		mov		DWORD PTR [esp-8], eax

		fld		DWORD PTR [esp-8] ;r
		fmul	st, st            ;r*r
		fld		DWORD PTR [esp-8] ;r
		fxch	st(1)
		fmul	DWORD PTR [esp+4];a ;r*r*y0
		fld		DWORD PTR [esp-12];load 1.5
		fld		st(0)
		fsub	st,st(2)			   ;r1 = 1.5 - y1
		;x1 = st(3)
		;y1 = st(2)
		;1.5 = st(1)
		;r1 = st(0)

		fld		st(1)
		fxch	st(1)
		fmul	st(3),st			; y2=y1*r1*...
		fmul	st(3),st			; y2=y1*r1*r1
		fmulp	st(4),st            ; x2=x1*r1
		fsub	st,st(2)               ; r2=1.5-y2
		;x2=st(3)
		;y2=st(2)
		;1.5=st(1)
		;r2 = st(0)

		fmul	st(2),st			;y3=y2*r2*...
		fmul	st(3),st			;x3=x2*r2
		fmulp	st(2),st			;y3=y2*r2*r2
		fxch	st(1)
		fsubp	st(1),st			;r3= 1.5 - y3
		;x3 = st(1)
		;r3 = st(0)
		fmulp	st(1), st
		fstp	a
	}

	return a;
}

/* courtsey: fastlib */
/* asm version of abs */
_n_inline f32  unsafeasmabsr(f32 a) 
{
	__asm
	{
		fld 	DWORD PTR [esp+4] 
		fabs
		ret 4
	}
}

/* asm version of abs */
_n_inline f32  asmabsr(f32 a) 
{
	__asm
	{
		fld 	a
		fabs
		fstp	a
	}

	return a;
}

/* asmfloorr : override */
_n_inline s32 __stdcall asmfloorr( f32 x )
{
	static f32 half = 0.5;
	s32 i; 
	__asm fld [x]
	__asm fsub [half]
	__asm fistp [i]
	return i;
}
_n_inline f32  unsafeasmsinr(f32 a) 
{
	__asm 
	{
		fld		DWORD PTR [esp+4] 
		fsin
		ret 4
	}
}
// asm version of sinf()
_n_inline f32  asmsinr(f32 a) 
{
	__asm 
	{
		fld		[a]
		fcos
		fstp	[a]
	}
	return a;
}
// asm version of cosf()
_n_inline f32  unsafeasmcosr(f32 a) 
{
	__asm 
	{
		fld		DWORD PTR [esp+4] 
		fcos
		ret 4
	}
}

// asm version of cosf()
_n_inline f32  asmcosr(f32 a) 
{
	__asm 
	{
		fld		[a]
		fcos
		fstp	[a]
	}
	return a;
}

// asm version of tanf()
_n_inline f32  asmtanr(f32 a)
{
	/* never checks if a == pi/2 */
	__asm
	{
		fld		a
		fcos
		fld		a
		fsin
		fdiv	st(0), st(1)
		fstp	a
	}

	return a;
}

#endif // X_USE_ASM_ALTERNATIVES

_n_inline f32 fsqrtr( f32 v ) {

#ifdef _MSC_VER
	
	f32 retval;

	__asm 
	{
		mov             eax, v
		sub             eax, 0x3F800000
		sar             eax, 1
		add             eax, 0x3F800000
		mov             [retval], eax
	}
	return retval;

#elif defined( _M_ALPHA ) || defined( _M_MRX000 ) || defined(__INTEL_COMPILER)
		
	return sqrtf(v);

#else

	return (f32)sqrt((double)v);

#endif
}


/* courtsey: fastlib */
/* floorr : override */
_n_inline s32 __stdcall floorr( f32 x )
{
	u32 e = (0x7F + 31) - ((* (u32 *) &x & 0x7F800000) >> 23);
	u32 m = 0x80000000 | (* (u32 *) &x << 8);
	return (m >> e) & -(e < 32);
}

//! Computes 1.0f / sqrt(x).
// Fast reciprocal square root (Quake 3 game code)
__inline f32 rsqrtr1(f32 number)
{
	long i;
	f32 x2, y;
	const f32 threehalfs = 1.5f;

	x2 = number * 0.5f;
	y  = number;
	i  = * (long *) &y;
	i  = 0x5f3759df - (i >> 1);
	y  = * (f32 *) &i;
	// Iteration 1 ...
	y  = y * (threehalfs - (x2 * y * y));
	// Result
	return y;
}

//! Saturates positive to zero.
_n_inline f32 satr( f32 f )
{
	u32 y = X_MkInt(f) & ~(X_MkSInt(f) >>31);
	return X_MkFloat(y);
}

_n_inline f32  sinc(f32 a)
{
	/* this is no asm */
	if(fabsr(a) < (f32)(1.0e-4)) return 1.0f - a*a*X_Real(0.166666666666666666667);
	return sinr(a);
}

_n_inline f32 fmax3(f32 a,f32 b,f32 c)
{
	f32 v;
#ifdef _MSC_VER
	_asm	fld		[a]
	_asm	fld		[b]
	_asm	fld		[c]
	_asm	_emit	0xdb	
	_asm	_emit	0xf1
	_asm	_emit	0xda	
	_asm	_emit	0xc1
	_asm	_emit	0xdb	
	_asm	_emit	0xf2
	_asm	_emit	0xda	
	_asm	_emit	0xc2
	_asm	fstp	[v]
	_asm	fcompp
#else
	v = x;
	if(v < y)  v = y;
	if(v < z)  v = z;
#endif
}

_n_inline f32 fmin3(f32 a,f32 b,f32 c)
{
	f32 v;
#ifdef _MSC_VER
	_asm	fld		[a]
	_asm	fld		[b]
	_asm	fld		[c]
	_asm	_emit	0xdb	
	_asm	_emit	0xf1
	_asm	_emit	0xdb	
	_asm	_emit	0xc1
	_asm	_emit	0xdb	
	_asm	_emit	0xf2
	_asm	_emit	0xdb	
	_asm	_emit	0xc2
	_asm	fstp	[v]
	_asm	fcompp
#else
	v = x;
	if(v > y)  v = y;
	if(v > z)  v = z;
#endif
}

#ifdef _MSC_VER
#	pragma warning( default : 4035 )
#endif

#endif XMATH_INL