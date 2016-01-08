//@ Abhishek Dey
//@ 1/17/2007

#include <xmath_c.h>
#ifdef X_SSE_INCLUDE
#ifdef XSSE_INTRIN_VERSION

#ifdef _m128c
#undef _m128c
#endif

#ifdef _lsing
#error Change the name '_lsing' in this file
#endif

#define _m128c(o) (*(__m128*)(o))
#define _lsing(reg,ad) 	\
	reg = _mm_load_ss((f32*)(ad)); \
	reg = _mm_shuffle_ps(reg,reg,_MM_SHUFFLE(0,0,0,0));

void nQuatMul(f32* qa,const f32* qb,const f32* qc)
{
	register __m128 d;
	d = _mm_mul_ps(_m128c(qb),_m128c(qc));

	_mm_store_ss(qa+0,_mm_sub_ss(_mm_sub_ss(_mm_sub_ss(d,_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,2,1))),_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,3,2))),_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,3,3))));

	d = _mm_mul_ps(_m128c(qb),_mm_shuffle_ps(_m128c(qc),_m128c(qc),_MM_SHUFFLE(2,3,0,1)));
	_mm_store_ss(qa+1,_mm_sub_ss(_mm_add_ss(_mm_add_ss(d,_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,2,1))),_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,3,2))),_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,3,3))));

	d = _mm_mul_ps(_m128c(qb),_mm_shuffle_ps(_m128c(qc),_m128c(qc),_MM_SHUFFLE(1,0,3,2)));
	_mm_store_ss(qa+2,_mm_add_ss(_mm_add_ss(_mm_sub_ss(d,_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,2,1))),_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,3,2))),_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,3,3))));

	d = _mm_mul_ps(_m128c(qb),_mm_shuffle_ps(_m128c(qc),_m128c(qc),_MM_SHUFFLE(0,1,2,3)));
	_mm_store_ss(qa+3,_mm_add_ss(_mm_sub_ss(_mm_add_ss(d,_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,2,1))),_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,3,2))),_mm_shuffle_ps(d,d,_MM_SHUFFLE(3,3,3,3))));
}

#endif
#endif