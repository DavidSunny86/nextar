#ifndef NEXTAR_MATH_QUAD_INL
#define NEXTAR_MATH_QUAD_INL

#include "NexSSEConstants.h"

#ifndef NEXTAR_MATH_SSETYPES_H
#error Do not include this file directly, Include  NexMath.h
#endif

/**
 * Layout: Rs:  [ [0] [1] [2] [3] ]
 *              [  x   y   z   w  ]
 */

_NexInline void QuadSinCos(Quad& vs, Quad& vc, const float val) {
#if defined( NEX_CORE_USE_ASM ) && defined( NEX_MSVC )
	__asm
	{
		fld val
		fsincos
		mov eax, vc
		fstp [eax]
		movaps xmm0, [eax]
		shufps xmm0, xmm0, 0
		movaps [eax], xmm0
		mov eax, vs
		fstp [eax]
		movaps xmm0, [eax]
		shufps xmm0, xmm0, 0
		movaps [eax], xmm0
	}
#else
	float s, c;
	Math::SinCos(val, s, c);
	vs = _mm_set_ps1(s);
	vc = _mm_set_ps1(c);
#endif
}

/** load 3 unaligned floats **/
_NexInline Quad QuadLoad3UF(const float* vals) {
	__m128 x = _mm_load_ss(vals);
	__m128 y = _mm_load_ss(vals + 1);
	__m128 z = _mm_load_ss(vals + 2);
	__m128 xy = _mm_unpacklo_ps(x, y);
	return _mm_movelh_ps(xy, z);
}

/** load 4 unaligned floats **/
_NexInline Quad QuadLoad4UF(const float* vals) {
	return _mm_loadu_ps(vals);
}

/** load 3 aligned floats **/
_NexInline Quad QuadLoad3AF(const float* vals) {
	return _mm_load_ps(vals);
}

/** load 4 aligned floats **/
_NexInline Quad QuadLoad4AF(const float* vals) {
	return _mm_load_ps(vals);
}

_NexInline bool QuadGreaterAny(QuadPF q1, QuadPF q2) {
	return (_mm_movemask_ps(_mm_cmpgt_ps(q1, q2))) != 0;
}

_NexInline bool QuadGreaterAll(QuadPF q1, QuadPF q2) {
	return (_mm_movemask_ps(_mm_cmpgt_ps(q1, q2)) == 0xF);
}

_NexInline bool QuadLesserAny(QuadPF q1, QuadPF q2) {
	return (_mm_movemask_ps(_mm_cmplt_ps(q1, q2))) != 0;
}

_NexInline bool QuadLesserAll(QuadPF q1, QuadPF q2) {
	return (_mm_movemask_ps(_mm_cmplt_ps(q1, q2)) == 0xF);
}

_NexInline Quad QuadAbs(QuadPF q) {
	return _mm_and_ps(q, N3D_ClearSign.v);
}

_NexInline Quad QuadNegate(QuadPF q) {
	return _mm_xor_ps(q, N3D_XXXX.v);
}

_NexInline Quad QuadAdd(QuadPF a, QuadPF b) {
	return _mm_add_ps(a, b);
}

_NexInline Quad QuadSub(QuadPF a, QuadPF b) {
	return _mm_sub_ps(a, b);
}

_NexInline Quad QuadMul(QuadPF a, QuadPF b) {
	return _mm_mul_ps(a, b);
}

_NexInline Quad QuadDiv(QuadPF a, QuadPF b) {
	return _mm_div_ps(a, b);
}

_NexInline Quad QuadMulScalar(QuadPF q, float val) {
	__m128 res = _mm_set_ps1(val);
	return _mm_mul_ps(q, res);
}

_NexInline Quad QuadZero() {
	return _mm_setzero_ps();
}

_NexInline Quad QuadSet(float x, float y, float z, float w) {
	return _mm_set_ps(w, z, y, x);
}

_NexInline Quad QuadSetX(QuadPF q, float val) {
	Quad v = _mm_set_ss(val);
	return _mm_move_ss(q, v);
}

_NexInline Quad QuadSetY(QuadPF q, float val) {
	Quad res = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 2, 0, 1));
	Quad v = _mm_set_ss(val);

	return _mm_move_ss(q, v);
}

_NexInline Quad QuadSetZ(QuadPF q, float val) {
	Quad v = _mm_set_ss(val);
	return _mm_move_ss(q, v);
}

_NexInline float QuadGetByIdx(QuadPF q, size_t idx) {
#if defined(NEX_MSVC)
	return q.m128_f32[idx];
#else
	return reinterpret_cast<const float*>(&q)[idx];
#endif
}

_NexInline float QuadGetX(QuadPF q) {
	return _mm_cvtss_f32(q);
}

_NexInline float QuadGetY(QuadPF q) {
	Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 1, 1, 1));
	return _mm_cvtss_f32(temp);
}

_NexInline float QuadGetZ(QuadPF q) {
	Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(2, 2, 2, 2));
	return _mm_cvtss_f32(temp);
}

_NexInline float QuadGetW(QuadPF q) {
	Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 3, 3, 3));
	return _mm_cvtss_f32(temp);
}

_NexInline Quad QuadSplatX(QuadPF q) {
	return _mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 0, 0, 0));
}

_NexInline Quad QuadSplatY(QuadPF q) {
	return _mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 1, 1, 1));
}

_NexInline Quad QuadSplatZ(QuadPF q) {
	return _mm_shuffle_ps(q, q, _MM_SHUFFLE(2, 2, 2, 2));
}

_NexInline Quad QuadSplatW(QuadPF q) {
	return _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 3, 3, 3));
}

_NexInline Quad QuadReplicate(float value) {
	return _mm_set_ps1(value);
}

_NexInline Quad QuadMulAdd(QuadPF a, QuadPF v, QuadPF c) {
	Quad t = _mm_mul_ps(a, v);
	return _mm_add_ps(t, c);
}

_NexInline Quad QuadInvSqrt(QuadPF qpf) {
#ifdef NEX_PREFER_SPEED_OVER_ACCURACY
	return __mm_rsqrt_ps(qpf);
#else
	const __m128 approx = _mm_rsqrt_ps(qpf);
	const __m128 muls = _mm_mul_ps(_mm_mul_ps(qpf, approx), approx);
	return _mm_mul_ps(_mm_mul_ps((N3D_HALF.v), approx), _mm_sub_ps((N3D_THREE.v), muls));
#endif
}

_NexInline Quad QuadSelect(QuadPF v1, QuadPF v2, QuadPF control) {
	Quad vtemp1 = _mm_andnot_ps(control, v1);
	Quad vtemp2 = _mm_and_ps(v2, control);
	return _mm_or_ps(vtemp1, vtemp2);
}

_NexInline Quad QuadIsInfinite(QuadPF v) {
    // Mask off the sign bit
    Quad vtemp = _mm_and_ps(v, N3D_ClearSign.v);
    // Compare to infinity
    vtemp = _mm_cmpeq_ps(vtemp, N3D_Infinite.v);
    // If any are infinity, the signs are true.
    return vtemp;
}

inline Quad QuadEqualInt(Quad q, Quad e) {
	__m128i v = _mm_cmpeq_epi32( reinterpret_cast<const __m128i *>(&q)[0],reinterpret_cast<const __m128i *>(&e)[0] );
	return reinterpret_cast<__m128 *>(&v)[0];
}


#endif //NEXTAR_MATH_QUAD_INL
