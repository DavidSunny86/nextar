//@ Abhishek Dey
//@ June 27,2006
//@ September 04 2007
//@ October 24 2007
//===========================================================================
// This file contains C specific defn, an alternative
// to the SSE library, in case it is not supported
// All functions in this file are tested

#include <xmmintrin.h>

#ifdef _lsing
#error Change the name '_lsing' in this file
#endif
#ifndef _m128c
#define _m128c(ptr) (*(__m128*)(ptr))
#endif

#define _lsing(reg,ad) 	\
	reg = _mm_load_ss((f32*)(ad)); \
	reg = _mm_shuffle_ps(reg,reg,_MM_SHUFFLE(0,0,0,0));

#ifndef xM12CpyMac
#define xM12CpyMac(m1,m2) \
	_m128c(m1+0) = _mm_load_ps((const f32*)(m2+0)); \
	_m128c(m1+4) = _mm_load_ps((const f32*)(m2+4)); \
	_m128c(m1+8) = _mm_load_ps((const f32*)(m2+8)); //SAFE_PTR_USAGE
#endif

#ifndef xM16CpyMac
#define xM16CpyMac(m1,m2) \
	_m128c(m1+0) = _mm_load_ps((const f32*)(m2+0)); \
	_m128c(m1+4) = _mm_load_ps((const f32*)(m2+4)); \
	_m128c(m1+8) = _mm_load_ps((const f32*)(m2+8)); \
	_m128c(m1+12) = _mm_load_ps((const f32*)(m2+12)); //SAFE_PTR_USAGE
#endif

#ifndef xQ4CpyMac
#define xQ4CpyMac(q1,q2) _m128c(q1) = _mm_load_ps((const f32*)(q2));
#endif

// just define these as macros
#define xM12Cpy	xM12CpyMac
#define xM16Cpy xM16CpyMac
#define xQ4Cpy	xQ4CpyMac

// Add
_n_inline void xQ4Add(f32* store,const f32* v1,const f32* v2)
{
	_m128c(store) = _mm_add_ps( _m128c(v1), _m128c(v2) );
}

// Sub
_n_inline void xQ4Sub(f32* store,const f32* v1,const f32* v2)
{
	_m128c(store) = _mm_sub_ps( _m128c(v1), _m128c(v2) );
}

// Mul
_n_inline void xQ4Mul(f32* store,const f32* v1,const f32* v2)
{
	_m128c(store) = _mm_mul_ps( _m128c(v1), _m128c(v2) );
}

// Div
_n_inline void xQ4Div(f32* store,const f32* v1,const f32* v2)
{
	_m128c(store) = _mm_div_ps( _m128c(v1), _m128c(v2) );
}

// scale
_n_inline void xQ4ScaleS(f32* store,const f32* v,f32 val)
{
	register __m128 xmm1;
	_lsing(xmm1,(&val));
	_m128c(store) = _mm_mul_ps(xmm1,_m128c(v));
}
// scale
_n_inline void xQ3ScaleS(f32* store,const f32* v,f32 val)
{
	register __m128 xmm1;
	xmm1 = _mm_load_ss((f32*)(v));
	xmm1 = _mm_shuffle_ps(xmm1,xmm1,_MM_SHUFFLE(3,0,0,0));
	_m128c(store) = _mm_mul_ps(xmm1,_m128c(v));

}
// scale
_n_inline void xQ4Scale(f32* v,f32 val)
{
	register __m128 xmm1;
	_lsing(xmm1,(&val));
	_m128c(v) = _mm_mul_ps(xmm1,_m128c(v));
}
// scale
_n_inline void xQ3Scale(f32* v,f32 val)
{
	register __m128 xmm1;
	xmm1 = _mm_load_ss((f32*)(v));
	xmm1 = _mm_shuffle_ps(xmm1,xmm1,_MM_SHUFFLE(3,0,0,0));
	_m128c(store) = _mm_mul_ps(xmm1,_m128c(v));
}
// i.e store += scale*vec v3,v4 ans:v3
_n_inline void xQ3ScaleNAdd(f32* store,const f32* q,f32 val)
{
	register __m128 xmm0;
	xmm0 = _mm_load_ss((f32*)(q));
	xmm0 = _mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(3,0,0,0));
	_mm_store_ps((f32*)store,_mm_add_ps(_m128c(store),_mm_mul_ps(xmm0,_m128c(vec))));
}
// i.e store += scale*vec v3,v4 ans:v3
_n_inline void xQ4ScaleNAdd(f32* store,const f32* q,f32 val)
{
	register __m128 xmm0;
	xmm0 = _mm_load_ss((f32*)(q));
	xmm0 = _mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(0,0,0,0));
	_mm_store_ps((f32*)store,_mm_add_ps(_m128c(store),_mm_mul_ps(xmm0,_m128c(vec))));
}
// i.e store -= scale*vec v3,v4 ans:v3
_n_inline void xQ3ScaleNSub(f32* store,const f32* q,f32 val)
{
	register __m128 xmm0;
	xmm0 = _mm_load_ss((f32*)(q));
	xmm0 = _mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(3,0,0,0));
	_mm_store_ps((f32*)store,_mm_sub_ps(_m128c(store),_mm_mul_ps(xmm0,_m128c(vec))));
}
// i.e store -= scale*vec v3,v4 ans:v3
_n_inline void xQ4ScaleNSub(f32* store,const f32* q,f32 val)
{
	register __m128 xmm0;
	xmm0 = _mm_load_ss((f32*)(q));
	xmm0 = _mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(0,0,0,0));
	_mm_store_ps((f32*)store,_mm_sub_ps(_m128c(store),_mm_mul_ps(xmm0,_m128c(vec))));
}
_n_inline void xQ3ScaleNMul(f32* store,const f32* q,f32 val)
{
	register __m128 xmm0;
	xmm0 = _mm_load_ss((f32*)(q));
	xmm0 = _mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(3,0,0,0));
	_mm_store_ps((f32*)store,_mm_mul_ps(_m128c(store),_mm_mul_ps(xmm0,_m128c(vec))));
}
_n_inline void xQ4ScaleNMul(f32* store,const f32* q,f32 val)
{
	register __m128 xmm0;
	xmm0 = _mm_load_ss((f32*)(q));
	xmm0 = _mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(0,0,0,0));
	_mm_store_ps((f32*)store,_mm_mul_ps(_m128c(store),_mm_mul_ps(xmm0,_m128c(vec))));
}
_n_inline void xQ3ScaleNDiv(f32* store,const f32* q,f32 val)
{
	register __m128 xmm0;
	xmm0 = _mm_load_ss((f32*)(q));
	xmm0 = _mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(3,0,0,0));
	_mm_store_ps((f32*)store,_mm_mul_ps(_m128c(store),_mm_div_ps(xmm0,_m128c(vec))));
}
_n_inline void xQ4ScaleNDiv(f32* store,const f32* q,f32 val)
{
	register __m128 xmm0;
	xmm0 = _mm_load_ss((f32*)(q));
	xmm0 = _mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(0,0,0,0));
	_mm_store_ps((f32*)store,_mm_mul_ps(_m128c(store),_mm_div_ps(xmm0,_m128c(vec))));
}
// TESTED
// Mul1_344
_n_inline void xMul1_344(f32* store,const f32* a,const f32* r)
{
	//@==load up 3 rows of r
	register __m128 xmm3,xmm4;
	//@==load a single element a into xmm3
	_lsing(xmm3,a);
	//@==multiply and store
	xmm4 = _mm_mul_ps(_m128c(r),xmm3);
	//@==load a single element(1) a into xmm3
	_lsing(xmm3,a+1);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((r+4)),xmm3));
	//@==load a single element(2) a into xmm3
	_lsing(xmm3,a+2);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((r+8)),xmm3));
	//@==store it as the first row
	_mm_store_ps((f32*)(store),xmm4);
}

// TESTED
// Mul0_344
_n_inline void xMul0_344(f32* store,const f32* a,const f32* r)
{
	register __m128 xmm1,xmm2;
	//xmm1 = _mm_load_ps((f32*)(r));
	xmm2 = _mm_load_ps((f32*)(a));
	xmm1 = _mm_mul_ps(_m128c(r),xmm2);
	_mm_store_ss((store),_mm_add_ss(_mm_shuffle_ps(xmm1, xmm1, _MM_SHUFFLE(3,3,3,2)),_mm_add_ss(_mm_shuffle_ps(xmm1, xmm1, _MM_SHUFFLE(3,3,3,1)),xmm1)));
	xmm1 = _mm_mul_ps(_m128c(((r+4))),xmm2);
	_mm_store_ss((store+1),_mm_add_ss(_mm_shuffle_ps(xmm1, xmm1, _MM_SHUFFLE(3,3,3,2)),_mm_add_ss(_mm_shuffle_ps(xmm1, xmm1, _MM_SHUFFLE(3,3,3,1)),xmm1)));
	xmm1 = _mm_mul_ps(_m128c((r+8)),xmm2);
	_mm_store_ss((store+2),_mm_add_ss(_mm_shuffle_ps(xmm1, xmm1, _MM_SHUFFLE(3,3,3,2)),_mm_add_ss(_mm_shuffle_ps(xmm1, xmm1, _MM_SHUFFLE(3,3,3,1)),xmm1)));
}

// multiply each row of r with a, add each COLOUMN of r and store in store
_n_inline void xMul2_344(f32* store,const f32* a,const f32* r)
{
	register __m128 xmm1,xmm2;
	xmm2 = _mm_load_ps((f32*)(a));
	_m128c(store) = _mm_mul_ps(xmm2,_mm_add_ps(_mm_add_ps(_m128c(r)_m128c(r+4)),_m128c(r+8)));
}

// TESTED
// Mul2_34
_n_inline void xMul2_34(f32* store,f32* a,f32* b)
{
	//@==load up 3 rows of b
	register __m128 xmm3,xmm4;
	//@==load a single element a into xmm3
	_lsing(xmm3,a);
	//@==multiply and store
	xmm4 = _mm_mul_ps(_m128c(b),xmm3);
	//@==load a single element(1) a into xmm3
	_lsing(xmm3,a+4);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+4)),xmm3));
	//@==load a single element(2) a into xmm3
	_lsing(xmm3,a+8);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+8)),xmm3));
	//@==store it as the first row
	_mm_store_ps((f32*)(store),xmm4);

	//@==load a single element a into xmm3
	_lsing(xmm3,a+1);
	//@==multiply and store
	xmm4 = _mm_mul_ps(_m128c(b),xmm3);
	//@==load a single element(1) a into xmm3
	_lsing(xmm3,a+5);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+4)),xmm3));
	//@==load a single element(2) a into xmm3
	_lsing(xmm3,a+9);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+8)),xmm3));
	//@==store it as the first row
	_mm_store_ps((f32*)(store+4),xmm4);
	
	//@==load a single element a into xmm3
	_lsing(xmm3,a+2);
	//@==multiply and store
	xmm4 = _mm_mul_ps(_m128c(b),xmm3);
	//@==load a single element(1) a into xmm3
	_lsing(xmm3,a+6);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+4)),xmm3));
	//@==load a single element(2) a into xmm3
	_lsing(xmm3,a+10);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+8)),xmm3));
	//@==store it as the first row
	_mm_store_ps((f32*)(store+8),xmm4);
}

// TESTED
_n_inline void xMul1_34(f32* store,const f32* a,const f32* r)
{
	//@==load up 3 rows of r
	register __m128 xmm3,xmm4;
	//@==load a single element a into xmm3
	_lsing(xmm3,a);
	//@==multiply and store
	xmm4 = _mm_mul_ps(_m128c(r),xmm3);
	//@==load a single element(1) a into xmm3
	_lsing(xmm3,a+1);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((r+4)),xmm3));
	//@==load a single element(2) a into xmm3
	_lsing(xmm3,a+2);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((r+8)),xmm3));
	//@==store it as the first row
	_mm_store_ps((f32*)(store),xmm4);

	//@==load a single element a into xmm3
	_lsing(xmm3,a+4);
	//@==multiply and store
	xmm4 = _mm_mul_ps(_m128c(r),xmm3);
	//@==load a single element(1) a into xmm3
	_lsing(xmm3,a+5);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((r+4)),xmm3));
	//@==load a single element(2) a into xmm3
	_lsing(xmm3,a+6);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((r+8)),xmm3));
	//@==store it as the first row
	_mm_store_ps((f32*)(store+4),xmm4);
	
	//@==load a single element a into xmm3
	_lsing(xmm3,a+8);
	//@==multiply and store
	xmm4 = _mm_mul_ps(_m128c(r),xmm3);
	//@==load a single element(1) a into xmm3
	_lsing(xmm3,a+9);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((r+4)),xmm3));
	//@==load a single element(2) a into xmm3
	_lsing(xmm3,a+10);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((r+8)),xmm3));
	//@==store it as the first row
	_mm_store_ps((f32*)(store+8),xmm4);
}

//TESTED
_n_inline void xTMulEfficient(transform* store,const transform* offset,const transform* parent)
{
	//@==load up 3 rows of r
	register __m128 xmm3,xmm4;
	//@==load a single element a into xmm3
	_lsing(xmm3,offset->pos);
	//@==multiply and store
	xmm4 = _mm_mul_ps(_m128c(parent->rot),xmm3);
	//@==load a single element(1) a into xmm3
	_lsing(xmm3,(offset->pos+1));
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((parent->rot+4)),xmm3));
	//@==load a single element(2) a into xmm3
	_lsing(xmm3,(offset->pos+2));
	//@==multiply and store and add
	xmm4 = _mm_add_ps(_m128c(parent->pos),_mm_add_ps(xmm4,_mm_mul_ps(_m128c((parent->rot+8)),xmm3)));
	//@==store it as the first row
	_mm_store_ps((f32*)(store->pos),xmm4);

	//@==load up 3 rows of r
	//@==load a single element a into xmm3
	_lsing(xmm3,offset->rot);
	//@==multiply and store
	xmm4 = _mm_mul_ps(_m128c(parent->rot),xmm3);
	//@==load a single element(1) a into xmm3
	_lsing(xmm3,offset->rot+1);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((parent->rot+4)),xmm3));
	//@==load a single element(2) a into xmm3
	_lsing(xmm3,offset->rot+2);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((parent->rot+8)),xmm3));
	//@==store it as the first row
	_mm_store_ps((f32*)(store->rot),xmm4);

	//@==load a single element a into xmm3
	_lsing(xmm3,offset->rot+4);
	//@==multiply and store
	xmm4 = _mm_mul_ps(_m128c(parent->rot),xmm3);
	//@==load a single element(1) a into xmm3
	_lsing(xmm3,offset->rot+5);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((parent->rot+4)),xmm3));
	//@==load a single element(2) a into xmm3
	_lsing(xmm3,offset->rot+6);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((parent->rot+8)),xmm3));
	//@==store it as the first row
	_mm_store_ps((f32*)(store->rot+4),xmm4);
	
	//@==load a single element a into xmm3
	_lsing(xmm3,offset->rot+8);
	//@==multiply and store
	xmm4 = _mm_mul_ps(_m128c(parent->rot),xmm3);
	//@==load a single element(1) a into xmm3
	_lsing(xmm3,offset->rot+9);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((parent->rot+4)),xmm3));
	//@==load a single element(2) a into xmm3
	_lsing(xmm3,offset->rot+10);
	//@==multiply and store and add
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((parent->rot+8)),xmm3));
	//@==store it as the first row
	_mm_store_ps((f32*)(store->rot+8),xmm4);
}

// TESTED 
_n_inline f32 xPCalculatePointR(const f32* pl,const f32* point)
{
	register __m128 xmm0,xmm1,xmm2;
	register f32 retval;
	xmm2 = _m128c(pl);
	xmm0 = _mm_mul_ps(xmm2,_m128c(point));
	xmm1 = _mm_shuffle_ps(xmm2,xmm0,_MM_SHUFFLE(2,1,3,3));
	_mm_store_ss(&retval,_mm_add_ss(_mm_add_ss(_mm_add_ss(xmm0,xmm1),_mm_shuffle_ps(xmm1,xmm1,_MM_SHUFFLE(3,3,3,2))),_mm_shuffle_ps(xmm1,xmm1,_MM_SHUFFLE(3,3,3,3))));
	return retval;
}

// TESTED 
_n_inline void xPCalculatePoint(f32* store_ptr,const f32* pl,const f32* point)
{
	register __m128 xmm0,xmm1,xmm2;
	xmm2 = _m128c(pl);
	xmm0 = _mm_mul_ps(xmm2,_m128c(point));
	xmm1 = _mm_shuffle_ps(xmm2,xmm0,_MM_SHUFFLE(2,1,3,3));
	_mm_store_ss(store_ptr,_mm_add_ss(_mm_add_ss(_mm_add_ss(xmm0,xmm1),_mm_shuffle_ps(xmm1,xmm1,_MM_SHUFFLE(3,3,3,2))),_mm_shuffle_ps(xmm1,xmm1,_MM_SHUFFLE(3,3,3,3))));
}

//TESTED
_n_inline void xPNormalize(f32* q)
{
	register __m128 xmm0;
	xmm0 = _mm_mul_ps(_m128c(q),_m128c(q));
	xmm0 = _mm_sqrt_ss(_mm_add_ss(_mm_add_ss(xmm0,_mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(3,3,2,1))),_mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(3,3,2,2))));
	_m128c(q) = _mm_div_ps(_m128c(q),_mm_shuffle_ps(xmm0,xmm0,0));
}

//TESTED
_n_inline void xPNormalizeS(f32* q1,const f32* q)
{
	register __m128 xmm0;
	xmm0 = _mm_mul_ps(_m128c(q),_m128c(q));
	xmm0 = _mm_sqrt_ss(_mm_add_ss(_mm_add_ss(xmm0,_mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(3,3,2,1))),_mm_shuffle_ps(xmm0,xmm0,_MM_SHUFFLE(3,3,2,2))));
	_m128c(q1) = _mm_div_ps(_m128c(q),_mm_shuffle_ps(xmm0,xmm0,0));
}

_inline void xMul1_44(f32* store, const f32* a,const f32* b)
{
	/*==load up 4 rows of b*/ 
	register __m128 xmm3,xmm4;
	/*==load a single element a into xmm3*/ 
	_lsing(xmm3,a);
	/*==multiply and store*/
	xmm4 = _mm_mul_ps(_m128c(b),xmm3);
	/*==load a single element(1) a into xmm3*/ 
	_lsing(xmm3,a+1);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+4)),xmm3));
	/*==load a single element(2) a into xmm3*/ 
	_lsing(xmm3,a+2);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+8)),xmm3));
	/*==load a single element(3) a into xmm3*/ 
	_lsing(xmm3,a+3);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+12)),xmm3));
	/*==store it as the first row*/
	_mm_store_ps((f32*)(store),xmm4);

	/*==load a single element a into xmm3*/ 
	_lsing(xmm3,a+4);
	/*==multiply and store*/
	xmm4 = _mm_mul_ps(_m128c(b),xmm3);
	/*==load a single element(1) a into xmm3*/ 
	_lsing(xmm3,a+5);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+4)),xmm3));
	/*==load a single element(2) a into xmm3*/ 
	_lsing(xmm3,a+6);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+8)),xmm3));
	/*==load a single element(3) a into xmm3*/ 
	_lsing(xmm3,a+7);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+12)),xmm3));
	/*==store it as the first row*/
	_mm_store_ps((f32*)(store+4),xmm4);
	
	/*==load a single element a into xmm3*/ 
	_lsing(xmm3,a+8);
	/*==multiply and store*/
	xmm4 = _mm_mul_ps(_m128c(b),xmm3);
	/*==load a single element(1) a into xmm3*/ 
	_lsing(xmm3,a+9);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+4)),xmm3));
	/*==load a single element(2) a into xmm3*/ 
	_lsing(xmm3,a+10);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+8)),xmm3));
	/*==load a single element(3) a into xmm3*/ 
	_lsing(xmm3,a+11);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+12)),xmm3));
	/*==store it as the first row*/
	_mm_store_ps((f32*)(store+8),xmm4);

	/*==load a single element a into xmm3*/ 
	_lsing(xmm3,a+12);
	/*==multiply and store*/
	xmm4 = _mm_mul_ps(_m128c(b),xmm3);
	/*==load a single element(1) a into xmm3*/ 
	_lsing(xmm3,a+13);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+4)),xmm3));
	/*==load a single element(2) a into xmm3*/ 
	_lsing(xmm3,a+14);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+8)),xmm3));
	/*==load a single element(3) a into xmm3*/ 
	_lsing(xmm3,a+15);
	/*==multiply and store and add*/
	xmm4 = _mm_add_ps(xmm4,_mm_mul_ps(_m128c((b+12)),xmm3));
	/*==store it as the first row*/
	_mm_store_ps((f32*)(store+12),xmm4);
}