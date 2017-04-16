#pragma once
namespace nextar {
namespace Math {

inline bool TraitsBase<_Quad>::Equals(pref v1, pref v2) {
	type r = QuadOp::Sub(v1, v2);
	return (bool)(
		Math::Traits<float>::Equals(QuadOp::GetX(r), 0) &&
		Math::Traits<float>::Equals(QuadOp::GetY(r), 0) &&
		Math::Traits<float>::Equals(QuadOp::GetZ(r), 0) &&
		Math::Traits<float>::Equals(QuadOp::GetW(r), 0));
}

inline bool TraitsBase<_Quad>::IsNan(pref v) {
	return QuadOp::Hadd(QuadOp::IsNan(v)) != 0;
}

inline bool TraitsBase<_Quad>::IsInf(pref v) {
	return QuadOp::Hadd(QuadOp::IsInf(v)) != 0;
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::IsNan(pref v) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_cmpneq_ps(v, v);
#else
	return impl::Set(
		v.v[0] != v.v[0], 
		v.v[1] != v.v[1],
		v.v[2] != v.v[2], 
		v.v[3] != v.v[3]);
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::IsInf(pref v) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	// Mask off the sign bit
	_Quad vtemp = _mm_and_ps(v, N3D_ClearSign.v);
	// Compare to infinity
	vtemp = _mm_cmpeq_ps(vtemp, N3D_Infinite.v);
	// If any are infinity, the signs are true.
	return vtemp;

#else
	return impl::Set(
		Traits<base_type>::IsInf(v.v[0]),
		Traits<base_type>::IsInf(v.v[1]),
		Traits<base_type>::IsInf(v.v[2]),
		Traits<base_type>::IsInf(v.v[3]));
#endif
}


template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Set(base_type v) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_set_ps1(v);
#else
	type r;
	for (uint32 i = 0; i < traits::_count; ++i)
		r.v[i] = v;
	return r;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Set(const base_type *v) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_load_ps(v);
#else
	type r;
	for (uint32 i = 0; i < traits::_count; ++i)
		r.v[i] = v[i];
	return r;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Set(base_type x, base_type y, base_type z) {
	return Set(x, y, z, 0);
}


template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Set(base_type x, base_type y, base_type z, base_type w) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_set_ps(w, z, y, x);
#else
	type r = { x, y, z, w };
	return r;
#endif
}


template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::SetUnaligned(const base_type *v) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_loadu_ps(v);
#else
	type r;
	for (uint32 i = 0; i < traits::_count; ++i)
		r.v[i] = v[i];
	return r;
#endif
}


template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Zero() {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_setzero_ps();
#else
	type r = { 0, 0, 0, 0 };
#endif
}


template <typename U>
inline typename _VecOp<_Quad, U>::base_type _VecOp<_Quad, U>::Get(pref q, uint32 idx) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#if defined(NEX_MSVC)
	return q.m128_f32[idx];
#else
	return reinterpret_cast<const base_type*>(&q)[idx];
#endif
#else
	return q.v[idx];
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::base_type _VecOp<_Quad, U>::GetX(pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_cvtss_f32(q);
#else
	return q.x;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::base_type _VecOp<_Quad, U>::GetY(pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	_Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 1, 1, 1));
	return _mm_cvtss_f32(temp);
#else
	return q.y;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::base_type _VecOp<_Quad, U>::GetZ(pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	_Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(2, 2, 2, 2));
	return _mm_cvtss_f32(temp);
#else
	return q.z;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::base_type _VecOp<_Quad, U>::GetW(pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	_Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 3, 3, 3));
	return _mm_cvtss_f32(temp);
#else
	return q.w;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::SetX(pref q, base_type val) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	_Quad v = _mm_set_ss(val);
	return _mm_move_ss(q, v);
#else
	_Quad r = q;
	r.x = val;
	return r;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::SetY(pref q, base_type val) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#ifdef NEX_MSVC
	_Quad res;
	res.m128_f32[0] = q.m128_f32[0];
	res.m128_f32[1] = val;
	res.m128_f32[2] = q.m128_f32[2];
	res.m128_f32[3] = q.m128_f32[3];
	return res;
#else
	_Quad res = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 2, 0, 1));
	_Quad v = _mm_set_ss(val);
	// Replace the x component
	res = _mm_move_ss(res, v);
	// Swap y and x again
	return _mm_shuffle_ps(res, res, _MM_SHUFFLE(3, 2, 0, 1));
#endif
#else
	_Quad r = q;
	r.y = val;
	return r;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::SetZ(pref q, base_type z) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#ifdef NEX_MSVC
	_Quad res;
	res.m128_f32[0] = q.m128_f32[0];
	res.m128_f32[1] = q.m128_f32[1];
	res.m128_f32[2] = val;
	res.m128_f32[3] = q.m128_f32[3];
	return res;
#else
	_Quad res = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 0, 1, 2));
	_Quad v = _mm_set_ss(val);
	// Replace the x component
	res = _mm_move_ss(res, v);
	// Swap y and x again
	return _mm_shuffle_ps(res, res, _MM_SHUFFLE(3, 0, 1, 2));
#endif
#else
	_Quad r = q;
	r.z = val;
	return r;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::SetW(pref q, base_type val) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#ifdef NEX_MSVC
	_Quad res;
	res.m128_f32[0] = q.m128_f32[0];
	res.m128_f32[1] = q.m128_f32[1];
	res.m128_f32[2] = q.m128_f32[2];
	res.m128_f32[3] = val;
	return res;
#else
	_Quad res = _mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 2, 1, 3));
	_Quad v = _mm_set_ss(val);
	// Replace the x component
	res = _mm_move_ss(res, v);
	// Swap y and x again
	return _mm_shuffle_ps(res, res, _MM_SHUFFLE(3, 2, 0, 1));
#endif
#else
	_Quad r = q;
	r.w = val;
	return r;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::SplatX(pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 0, 0, 0));
#else
	return { q.x, q.x, q.x, q.x };
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::SplatY(pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 1, 1, 1));
#else
	return { q.y, q.y, q.y, q.y };
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::SplatZ(pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_shuffle_ps(q, q, _MM_SHUFFLE(2, 2, 2, 2));
#else
	return { q.z, q.z, q.z, q.z };
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::SplatW(pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 3, 3, 3));
#else
	return { q.w, q.w, q.w, q.w };
#endif
}

template <typename U>
inline bool _VecOp<_Quad, U>::GreaterAny(pref a, pref b) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return (_mm_movemask_ps(_mm_cmpgt_ps(a, b))) != 0;
#else
	return a.x > b.x || a.y > b.y || a.z > b.z || a.w > b.w;
#endif
}

template <typename U>
inline bool _VecOp<_Quad, U>::GreaterAll(pref a, pref b) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return (_mm_movemask_ps(_mm_cmpgt_ps(a, b)) == 0xF);
#else
	return a.x > b.x && a.y > b.y && a.z > b.z && a.w > b.w;
#endif
}

template <typename U>
inline bool _VecOp<_Quad, U>::LesserAny(pref a, pref b) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return (_mm_movemask_ps(_mm_cmplt_ps(a, b))) != 0;
#else
	return a.x < b.x || a.y < b.y || a.z < b.z || a.w < b.w;
#endif
}

template <typename U>
inline bool _VecOp<_Quad, U>::LesserAll(pref q1, pref q2) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return (_mm_movemask_ps(_mm_cmplt_ps(q1, q2)) == 0xF);
#else
	return a.x < b.x && a.y < b.y && a.z < b.z && a.w < b.w;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Abs(pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_and_ps(q, N3D_ClearSign.v);
#else
	return impl::Set(Math::Abs(q.x), Math::Abs(q.y), Math::Abs(q.z), Math::Abs(q.w));
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Negate(pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_xor_ps(q, N3D_XXXX.v);
#else
	return impl::Set(-(q.x), -(q.y), -(q.z), -(q.w));
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Add(pref a, pref b) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_add_ps(a, b);
#else
	return impl::Set(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Sub(pref a, pref b) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_sub_ps(a, b);
#else
	return impl::Set(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);

#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Mul(pref a, pref b) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_mul_ps(a, b);
#else
	return impl::Set(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Div(pref a, pref b) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_div_ps(a, b);
#else
	return impl::Set(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Mul(pref q, base_type val) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	__m128 res = _mm_set_ps1(val);
	return _mm_mul_ps(q, res);
#else
	return impl::Set(q.x * val, q.y * val, q.z * val, q.w * val);
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Mul(base_type val, pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	__m128 res = _mm_set_ps1(val);
	return _mm_mul_ps(q, res);
#else
	return impl::Set(q.x * val, q.y * val, q.z * val, q.w * val);
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Madd(pref a, pref v, pref c) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	_Quad t = _mm_mul_ps(a, v);
	return _mm_add_ps(t, c);
#else 
	return impl::Add(impl::Mul(a, v), c);
#endif
}

template<typename U>
inline typename _VecOp<_Quad, U>::base_type nextar::Math::_VecOp<_Quad, U>::Hadd(pref v) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE3 || NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE4
	_Quad q = _mm_hadd_ps(v, v); // latency 7
	q = _mm_hadd_ps(q, q);// latency 7
	return GetX(q);
#else
	_Quad q = v;
	_Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 2, 3, 2));
	q = _mm_add_ps(q, temp);
	// x+z,x+z,x+z,y+w
	q = _mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 0, 0, 0));
	// y+w, ??, ??, ??
	temp = _mm_shuffle_ps(q, temp, _MM_SHUFFLE(0, 0, 0, 3));
	// x+z+y+w,??, ??, ??
	q = _mm_add_ss(q, temp);
	return q;
#endif
#else
	return v.x + v.y + v.z + v.w;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::InvSqrt(pref qpf) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#ifdef NEX_PREFER_SPEED_OVER_ACCURACY
	return __mm_rsqrt_ps(qpf);
#else
	const __m128 approx = _mm_rsqrt_ps(qpf);
	const __m128 muls = _mm_mul_ps(_mm_mul_ps(qpf, approx), approx);
	return _mm_mul_ps(_mm_mul_ps((N3D_HALF.v), approx),
		_mm_sub_ps((N3D_THREE.v), muls));
#endif
#else
	return impl::Set(RecipSqrt(qpf.x), RecipSqrt(qpf.y), RecipSqrt(qpf.z), RecipSqrt(qpf.w));
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Select(pref v1, pref v2, pref control) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	_Quad vtemp1 = _mm_andnot_ps(control, v1);
	_Quad vtemp2 = _mm_and_ps(v2, control);
	return _mm_or_ps(vtemp1, vtemp2);
#else
	_Quad ret;
	uint32* iret = reinterpret_cast<uint32*>(&ret);
	uint32* iv1 = reinterpret_cast<uint32*>(&v1);
	uint32* iv2 = reinterpret_cast<uint32*>(&v2);
	uint32* ic = reinterpret_cast<uint32*>(&control);
	for(int i = 0; i < 4; ++i)
		iret[i] = (~ic[i] & iv1[i]) | (ic[i] & iv2[i]);
	return ret;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::VDot(pref vec1, pref vec2) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE4
	return _mm_dp_ps(vec1, vec2, 0xFF);
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE3
	_Quad q = _mm_mul_ps(vec1, vec2);
	q = _mm_hadd_ps(q, q); // latency 7
	q = _mm_hadd_ps(q, q);// latency 7
	return q;
#else
	_Quad q = _mm_mul_ps(vec1, vec2);
	_Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 2, 3, 2));
	q = _mm_add_ps(q, temp);
	// x+z,x+z,x+z,y+w
	q = _mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 0, 0, 0));
	// y+w, ??, ??, ??
	temp = _mm_shuffle_ps(q, temp, _MM_SHUFFLE(0, 0, 0, 3));
	// x+z+y+w,??, ??, ??
	q = _mm_add_ss(q, temp);
	return q;
#endif
#else
	return impl::Set(impl::Dot(q1, q2), 0, 0, 0);
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::base_type _VecOp<_Quad, U>::Dot(pref q1, pref q2) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return impl::GetX(impl::VDot(q1, q2));
#else
	float r = 0;
	for (uint32 i = 0; i < 4; i++)
		r += q1.v[i] * q2.v[i];
	return r;
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Normalize(pref vec) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE4
	_Quad q = _mm_dp_ps(vec, vec, 0xFF);
	// Get the reciprocal
	q = _mm_sqrt_ps(q);
	return _mm_div_ps(vec, q);
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE3
	_Quad q = _mm_mul_ps(vec, vec);
	q = _mm_hadd_ps(q, q); // latency 7
	q = _mm_hadd_ps(q, q);// latency 7
						  // Get the reciprocal
	q = _mm_sqrt_ss(q);
	q = _mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 0, 0, 0));
	return _mm_div_ps(vec, q);
#else
	_Quad q = _mm_mul_ps(vec, vec);
	_Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 2, 3, 2));
	q = _mm_add_ps(q, temp);
	// x+z,x+z,x+z,y+w
	q = _mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 0, 0, 0));
	// y+w, ??, ??, ??
	temp = _mm_shuffle_ps(q, temp, _MM_SHUFFLE(0, 0, 0, 3));
	// x+z+y+w,??, ??, ??
	q = _mm_add_ss(q, temp);
	// Get the reciprocal
	q = _mm_sqrt_ss(q);
	q = _mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 0, 0, 0));
	return _mm_div_ps(vec, q);
#endif
#else
	float res = Math::Sqrt(impl::Dot(vec, vec));
	NEX_ASSERT(res > Math::EPSILON);
	return impl::Set(vec.x / res, vec.y / res, vec.z / res, vec.w / res);
#endif
}

template <typename U>
inline typename _VecOp<_Quad, U>::type _VecOp<_Quad, U>::Lerp(pref src, pref dst, float_type t) {
	return impl::Madd(impl::Set(t), impl::Sub(dst, src), src);
}

template <typename U>
inline typename _VecOp<_Quad, U>::float_type _VecOp<_Quad, U>::Length(pref vec) {
	return Math::Sqrt(impl::Dot(vec, vec));
}

template <typename U>
inline typename _VecOp<_Quad, U>::float_type _VecOp<_Quad, U>::SqLength(pref vec) {
	return impl::Dot(vec, vec);
}

template <typename U>
inline typename _VecOp<_Quad, U>::float_type _VecOp<_Quad, U>::Distance(pref vec1, pref vec2) {
	return impl::Length(impl::Sub(vec1, vec2));
}

template <typename U>
inline typename _VecOp<_Quad, U>::float_type _VecOp<_Quad, U>::SqDistance(pref vec1, pref vec2) {
	return impl::SqLength(impl::Sub(vec1, vec2));
}

}
}
