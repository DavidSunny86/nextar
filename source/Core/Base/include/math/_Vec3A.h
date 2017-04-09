#pragma once
using namespace nextar;
using namespace nextar::Math;


inline _VecOp<_Vec3A, _Vec3A>::type _VecOp<_Vec3A, _Vec3A>::Cross(pref vec1, pref vec2) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	// y1,z1,x1,w1
	Quad vTemp1 = _mm_shuffle_ps(vec1, vec1, _MM_SHUFFLE(3, 0, 2, 1));
	// z2,x2,y2,w2
	Quad vTemp2 = _mm_shuffle_ps(vec2, vec2, _MM_SHUFFLE(3, 1, 0, 2));
	// Perform the left operation
	Quad vResult = _mm_mul_ps(vTemp1, vTemp2);
	// z1,x1,y1,w1
	vTemp1 = _mm_shuffle_ps(vTemp1, vTemp1, _MM_SHUFFLE(3, 0, 2, 1));
	// y2,z2,x2,w2
	vTemp2 = _mm_shuffle_ps(vTemp2, vTemp2, _MM_SHUFFLE(3, 1, 0, 2));
	// Perform the right operation
	vTemp1 = _mm_mul_ps(vTemp1, vTemp2);
	// Subract the right from left, and return answer
	vResult = _mm_sub_ps(vResult, vTemp1);
	// Set w to zero
	return _mm_and_ps(vResult, N3D_FFFO.v);
#else
	return Set(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
#endif
}

inline _VecOp<_Vec3A, _Vec3A>::type _VecOp<_Vec3A, _Vec3A>::Normalize(pref vec) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE4
	Quad q = _mm_dp_ps(vec, vec, 0x7F);
	// Get the reciprocal
	q = _mm_sqrt_ps(q);
	return _mm_div_ps(vec, q);
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE3
	Quad q = _mm_mul_ps(vec, vec);
	q = _mm_and_ps(q, (N3D_FFFO.v));
	q = _mm_hadd_ps(q, q); // latency 7
	q = _mm_hadd_ps(q, q);// latency 7
						  // Get the reciprocal
	q = _mm_sqrt_ss(q);
	q = _mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 0, 0, 0));
	return _mm_div_ps(vec, q);
#else
	// Perform the dot product
	Quad q = _mm_mul_ps(vec, vec);
	// x=Dot.y, y=Dot.z
	Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(2, 1, 2, 1));
	// Result.x = x+y
	q = _mm_add_ss(q, temp);
	// x=Dot.z
	temp = _mm_shuffle_ps(temp, temp, _MM_SHUFFLE(1, 1, 1, 1));
	// Result.x = (x+y)+z
	q = _mm_add_ss(q, temp);
	// Splat x
	q = _mm_sqrt_ss(q);
	q = _mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 0, 0, 0));
	return _mm_div_ps(vec, q);
#endif
#else
	float val = Math::Sqrt(Dot(vec, vec));
	NEX_ASSERT(val > Math::EPSILON);
	return Mul(vec, 1 / val);
#endif
}

inline _VecOp<_Vec3A, _Vec3A>::type _VecOp<_Vec3A, _Vec3A>::VDot(pref vec1, pref vec2) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE4
	return _mm_dp_ps(vec1, vec2, 0x7F);
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE3
	Quad q = _mm_mul_ps(vec1, vec2);
	q = _mm_and_ps(q, (N3D_FFFO.v));
	q = _mm_hadd_ps(q, q); // latency 7
	q = _mm_hadd_ps(q, q);// latency 7
	return q;
#else
	// Perform the dot product
	Quad q = _mm_mul_ps(vec1, vec2);
	// x=Dot.y, y=Dot.z
	Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(2, 1, 2, 1));
	// Result.x = x+y
	q = _mm_add_ss(q, temp);
	// x=Dot.z
	temp = _mm_shuffle_ps(temp, temp, _MM_SHUFFLE(1, 1, 1, 1));
	// Result.x = (x+y)+z
	q = _mm_add_ss(q, temp);
	return q;
#endif
#else
	return Set(Dot(q1, q2), 0, 0);
#endif
}

inline _VecOp<_Vec3A, _Vec3A>::base_type _VecOp<_Vec3A, _Vec3A>::Dot(pref q1, pref q2) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return GetX(VDot(q1, q2));
#else
	float r = 0;
	for (uint32 i = 0; i < 3; i++)
		r += q1.v[i] * q2.v[i];
	return r;
#endif
}


inline bool _VecOp<_Vec3A, _Vec3A>::GreaterAny(pref q1, pref q2) {
	return ((_mm_movemask_ps(_mm_cmpgt_ps(q1, q2))) & 0x7) != 0;
}

inline bool _VecOp<_Vec3A, _Vec3A>::GreaterAll(pref q1, pref q2) {
	return ((_mm_movemask_ps(_mm_cmpgt_ps(q1, q2)) & 0x7) == 0x7);
}

inline bool _VecOp<_Vec3A, _Vec3A>::LesserAny(pref q1, pref q2) {
	return GreaterAny(q2, q1);
}

inline bool _VecOp<_Vec3A, _Vec3A>::LesserAll(pref q1, pref q2) {
	return GreaterAll(q2, q1);
}

inline _VecOp<_Vec3A, _Vec3A>::type _VecOp<_Vec3A, _Vec3A>::Mul(pref v, TraitsMat4x4::pref m) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	Quad ret;
	ret = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
	ret = _mm_mul_ps(ret, m.r[0]);
	Quad vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
	vTemp = _mm_mul_ps(vTemp, m.r[1]);
	ret = _mm_add_ps(ret, vTemp);
	vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
	vTemp = _mm_mul_ps(vTemp, m.r[2]);
	ret = _mm_add_ps(ret, vTemp);
	ret = _mm_add_ps(ret, m.r[3]);
	return ret;
#else
	Quad r, x, y, z;

	z = SplatZ(v);
	y = SplatY(v);
	x = SplatX(v);

	r = Madd(z, m(2), m(3));
	r = Madd(y, m(1), r);
	r = Madd(x, m(0), r);

	return r;
#endif
}

