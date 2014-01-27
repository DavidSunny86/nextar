#pragma once

#ifndef NEXTAR_MATH_SSETYPES_H
#	error Do not include this file directly, Include  NexMath.h
#endif

_NexInline Vector3A Vec3ASet(float x, float y, float z) {
	return QuadSet(x, y, z, 0);
}

_NexInline Vector3A Vec3AFromVec3(const Vector3& vec3) {
	return QuadLoad3UF(&vec3.x);
}

_NexInline float Vec3ADot(Vec3AF vec1, Vec3AF vec2) {
	float ret;
#if defined(NEX_VECTOR_MATH_SSE4)
	Quad q = _mm_dp_ps(vec1,vec2,0x7F);
	// Get the reciprocal
	_mm_store_ss(&ret,q);
#elif defined(NEX_VECTOR_MATH_SSE3)
	Quad q = _mm_mul_ps(vec1,vec2);
	q = _mm_and_ps(q,(N3D_FFFO.v));
	q = _mm_hadd_ps(q,q); // latency 7
	q = _mm_hadd_ps(q,q);// latency 7
	// Get the reciprocal
	_mm_store_ss(&ret,q);
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
	// Splat x
	_mm_store_ss(&ret, q);
#endif
	return ret;
}

/** @reamarks Normalize */
_NexInline Vector3A Vec3ANormalize(Vec3AF vec) {
#if defined(NEX_VECTOR_MATH_SSE4)
	Quad q = _mm_dp_ps(vec,vec,0x7F);
	// Get the reciprocal
	q = _mm_sqrt_ps(q);
	return _mm_div_ps(vec,q);
#elif defined(NEX_VECTOR_MATH_SSE3)
	Quad q = _mm_mul_ps(vec,vec);
	q = _mm_and_ps(q,(N3D_FFFO.v));
	q = _mm_hadd_ps(q,q); // latency 7
	q = _mm_hadd_ps(q,q);// latency 7
	// Get the reciprocal
	q = _mm_sqrt_ss(q);
	q = _mm_shuffle_ps(q, q,_MM_SHUFFLE(0,0,0,0));
	return _mm_div_ps(vec,q);
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
}

_NexInline bool Vec3AGreaterAny(Vec4AF q1, Vec4AF q2) {
	return ((_mm_movemask_ps(_mm_cmpgt_ps(q1, q2))) & 0x7) != 0;
}

_NexInline bool Vec3AGreaterAll(Vec4AF q1, Vec4AF q2) {
	return ((_mm_movemask_ps(_mm_cmpgt_ps(q1, q2)) & 0x7) == 0x7);
}

_NexInline bool Vec3ALesserAny(Vec4AF q1, Vec4AF q2) {
	return Vec3AGreaterAny(q2, q1);
}

_NexInline bool Vec3ALesserAll(Vec4AF q1, Vec4AF q2) {
	return Vec3AGreaterAll(q2, q1);
}

_NexInline Vector3A Vec3ACross(Vec3AF vec1, Vec3AF vec2) {
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
}

/** @reamarks Lerp */
_NexInline Vector3A Vec3ALerp(Vec3AF src, Vec3AF dst, float t) {
	/* @todo Make efficient */
	return Vec3AMulAdd(Vec4AReplicate(t), Vec3ASub(dst, src), src);
}

/** @todo */
_NexInline float Vec3ALength(Vec3AF vec) {
	return Math::Sqrt(Vec3ADot(vec, vec));
}

/** @todo */
_NexInline float Vec3ASqLength(Vec3AF vec) {
	return (Vec3ADot(vec, vec));
}

/** @brief **/
_NexInline float Vec3ADistance(Vec3AF vec1, Vec3AF vec2) {
	return Vec3ALength(Vec3ASub(vec1, vec2));
}

/** @brief **/
_NexInline float Vec3ASqDistance(Vec3AF vec1, Vec3AF vec2) {
	return Vec3ASqLength(Vec3ASub(vec1, vec2));
}
