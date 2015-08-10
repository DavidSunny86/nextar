
namespace nextar {

/** @reamarks Dot */
inline float Vec4ADot(Vec4AF vec1, Vec4AF vec2) {
	float ret;
#if defined(NEX_VECTOR_MATH_SSE4)
	Quad q = _mm_dp_ps(vec1,vec2,0xFF);
	// Get the reciprocal
	_mm_store_ss(&ret,q);
#elif defined(NEX_VECTOR_MATH_SSE3)
	Quad q = _mm_mul_ps(vec1,vec2);
	q = _mm_hadd_ps(q,q); // latency 7
	q = _mm_hadd_ps(q,q);// latency 7
	// Get the reciprocal
	_mm_store_ss(&ret,q);
#else
	Quad q = _mm_mul_ps(vec1, vec2);
	Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 2, 3, 2));
	q = _mm_add_ps(q, temp);
	// x+z,x+z,x+z,y+w
	q = _mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 0, 0, 0));
	// y+w, ??, ??, ??
	temp = _mm_shuffle_ps(q, temp, _MM_SHUFFLE(0, 0, 0, 3));
	// x+z+y+w,??, ??, ??
	q = _mm_add_ss(q, temp);
	// Get the reciprocal
	_mm_store_ss(&ret, q);
#endif
	return ret;
}

/** @reamarks Normalize */
inline Vector4A Vec4ANormalize(Vec4AF vec) {
#if defined(NEX_VECTOR_MATH_SSE4)
	Quad q = _mm_dp_ps(vec,vec,0xFF);
	// Get the reciprocal
	q = _mm_sqrt_ps(q);
	return _mm_div_ps(vec,q);
#elif defined(NEX_VECTOR_MATH_SSE3)
	Quad q = _mm_mul_ps(vec,vec);
	q = _mm_hadd_ps(q,q); // latency 7
	q = _mm_hadd_ps(q,q);// latency 7
	// Get the reciprocal
	q = _mm_sqrt_ss(q);
	q = _mm_shuffle_ps(q, q,_MM_SHUFFLE(0,0,0,0));
	return _mm_div_ps(vec,q);
#else
	Quad q = _mm_mul_ps(vec, vec);
	Quad temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 2, 3, 2));
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
}

/** @reamarks Lerp */
inline Vector4A Vec4ALerp(Vec4AF src, Vec4AF dst, float t) {
	/* todo Make efficient */
	return Vec4AMulAdd(Vec4AReplicate(t), Vec4ASub(dst, src), src);
}

/** todo */
inline float Vec4ASqLength(Vec4AF vec) {
	return (Vec4ADot(vec, vec));
}

/** todo*/
inline float Vec4ALength(Vec4AF vec) {
	return Math::Sqrt(Vec4ADot(vec, vec));
}

/** todo*/
inline float Vec4ADistance(Vec4AF vec1, Vec4AF vec2) {
	return Vec4ALength(Vec4ASub(vec1, vec2));
}

/** todo*/
inline float Vec4ASqDistance(Vec4AF vec1, Vec4AF vec2) {
	return Vec4ASqLength(Vec4ASub(vec1, vec2));
}

inline Vector4A Vec4AMulMat4x4(Vec4AF v, Mat4x4F m) {
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
}

}
