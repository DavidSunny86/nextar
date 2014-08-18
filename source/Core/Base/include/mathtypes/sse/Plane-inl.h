/* 
 * File:   _plane.h
 * Author: obhi
 *
 * Created on January 24, 2011, 8:12 PM
 */

#ifndef NEXTAR_CORE_PLANE_H
#define	NEXTAR_CORE_PLANE_H

namespace nextar {

#define PlaneNormalize			Vec3ANormalize
#define PlaneNormalDotVec3A     Vec3ADot
#define PlaneAbsNormal          QuadAbs

inline float PlaneDotVec3A(PlaneF p, Vec3AF v) {
	float ret;
#if defined(NEX_VECTOR_MATH_SSE4)
	Quad q = _mm_and_ps(v,N3D_FFFO.v);
	q = _mm_or_ps(q,N3D_0001.v);
	_mm_dp_ps(q,p,0xFF);
	// Get the reciprocal
	_mm_store_ss(&ret,q);
#elif defined(NEX_VECTOR_MATH_SSE3)
	Quad q = _mm_and_ps(v,(N3D_FFFO.v));
	q = _mm_or_ps(q,(N3D_0001.v));
	q = _mm_mul_ps(q,p);
	q = _mm_hadd_ps(q,q); // latency 7
	q = _mm_hadd_ps(q,q);// latency 7
	// Get the reciprocal
	_mm_store_ss(&ret,q);
#else
	Quad q = _mm_and_ps(v, N3D_FFFO);
	q = _mm_or_ps(q, N3D_0001);
	q = _mm_mul_ps(q, p);
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
}
#endif	/* NEXTAR_CORE_PLANE_H */

