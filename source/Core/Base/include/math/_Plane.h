#pragma once
namespace nextar {
namespace Math {

inline _VecOp<_Plane, _Plane>::type _VecOp<_Plane, _Plane>::Normalize(pref q) {
	return Vec3AOp::Normalize(q);
}

inline _VecOp<_Plane, _Plane>::float_type _VecOp<_Plane, _Plane>::Dot(pref p, TraitsVec3A::pref v) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE4
	_Quad q = _mm_and_ps(v, N3D_FFFO.v);
	q = _mm_or_ps(q, N3D_0001.v);
	_mm_dp_ps(q, p, 0xFF);
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE3
	_Quad q = _mm_and_ps(v, (N3D_FFFO.v));
	q = _mm_or_ps(q, (N3D_0001.v));
	q = _mm_mul_ps(q, p);
	q = _mm_hadd_ps(q, q); // latency 7
	q = _mm_hadd_ps(q, q);// latency 7
#else
	_Quad q = _mm_and_ps(v, N3D_FFFO.v);
	q = _mm_or_ps(q, N3D_0001.v);
	q = _mm_mul_ps(q, p);
	_Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 2, 3, 2));
	q = _mm_add_ps(q, temp);
	// x+z,x+z,x+z,y+w
	q = _mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 0, 0, 0));
	// y+w, ??, ??, ??
	temp = _mm_shuffle_ps(q, temp, _MM_SHUFFLE(0, 0, 0, 3));
	// x+z+y+w,??, ??, ??
	q = _mm_add_ss(q, temp);
#endif
	return GetX(q);
#else
	return p.x * v.x + p.y * v.y + p.z * v.z + p.w;
#endif
}

inline _VecOp<_Plane, _Plane>::float_type _VecOp<_Plane, _Plane>::DotNormal(pref plane, TraitsVec3A::pref q) {
	return Vec3AOp::Dot(plane, q);
}

inline _VecOp<_Plane, _Plane>::type _VecOp<_Plane, _Plane>::AbsNormal(pref plane) {
	return Vec3AOp::Abs(plane);
}

}
}
