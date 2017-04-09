#pragma once
using namespace nextar;
using namespace nextar::Math;


inline _VecOp<_Vec4, _Vec4>::type _VecOp<_Vec4, _Vec4>::Mul(pref v, TraitsMat4x4::pref m) {
#ifdef NEX_VECTOR_MATH_TYPE_IS_SSE
	Quad ret, vTemp;
	ret = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
	ret = _mm_mul_ps(ret, m.r[0]);
	vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
	vTemp = _mm_mul_ps(vTemp, m.r[1]);
	ret = _mm_add_ps(ret, vTemp);
	vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
	vTemp = _mm_mul_ps(vTemp, m.r[2]);
	ret = _mm_add_ps(ret, vTemp);
	vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));
	vTemp = _mm_mul_ps(vTemp, m.r[3]);
	ret = _mm_add_ps(ret, vTemp);
	return ret;
#else
	Quad r, x, y, z, w;

	z = SplatZ(v);
	y = SplatY(v);
	x = SplatX(v);
	w = SplatW(v);

	r = Mul(w, m(3));
	r = Madd(z, m(2), r);
	r = Madd(y, m(1), r);
	r = Madd(x, m(0), r);

	return r;
#endif
}
