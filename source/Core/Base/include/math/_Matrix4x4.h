#include "VectorTypes.h"
#pragma once
namespace nextar {
namespace Math {

inline _Matrix4x4::_Matrix4x4() {
}

inline _Matrix4x4& _Matrix4x4::operator =(const Matrix4x4& m) {
	r[0] = Mat4x4Op::Row(m, 0);
	r[1] = Mat4x4Op::Row(m, 1);
	r[2] = Mat4x4Op::Row(m, 2);
	r[3] = Mat4x4Op::Row(m, 3);
	return *this;
}

inline _Matrix4x4::_Matrix4x4(const Matrix3x4& m, const Vector3A& v) {
	r[0] = Mat3x4Op::Row(m, 0);
	r[1] = Mat3x4Op::Row(m, 1);
	r[2] = Mat3x4Op::Row(m, 2);
	r[3] = v;
}

inline _Matrix4x4::_Matrix4x4(float m00, float m01, float m02, float m03, float m10,
	float m11, float m12, float m13, float m20, float m21, float m22,
	float m23, float m30, float m31, float m32, float m33) {
	r[0] = Vec4::Set(m00, m01, m02, m03);
	r[1] = Vec4::Set(m10, m11, m12, m13);
	r[2] = Vec4::Set(m20, m21, m22, m23);
	r[3] = Vec4::Set(m30, m31, m32, m33);
}

inline _Matrix4x4::_Matrix4x4(std::initializer_list<float> l) {
	std::copy(l.begin(), l.end(), m);
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::Mul(pref m1, pref m2) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	Matrix4x4 result;
	// Use vw to hold the original row
	_Quad vw = m1.r[0];
	// Splat the component x,y,Z then W
	_Quad vx = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
	_Quad vy = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
	_Quad vz = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
	vw = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
	// Perform the opertion on the first row
	vx = _mm_mul_ps(vx, m2.r[0]);
	vy = _mm_mul_ps(vy, m2.r[1]);
	vz = _mm_mul_ps(vz, m2.r[2]);
	vw = _mm_mul_ps(vw, m2.r[3]);
	// Perform a binary Add to reduce cumulative errors
	vx = _mm_add_ps(vx, vz);
	vy = _mm_add_ps(vy, vw);
	vx = _mm_add_ps(vx, vy);
	result.r[0] = vx;
	// Repeat for the other 3 rows
	vw = m1.r[1];
	vx = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
	vy = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
	vz = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
	vw = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
	vx = _mm_mul_ps(vx, m2.r[0]);
	vy = _mm_mul_ps(vy, m2.r[1]);
	vz = _mm_mul_ps(vz, m2.r[2]);
	vw = _mm_mul_ps(vw, m2.r[3]);
	vx = _mm_add_ps(vx, vz);
	vy = _mm_add_ps(vy, vw);
	vx = _mm_add_ps(vx, vy);
	result.r[1] = vx;
	vw = m1.r[2];
	vx = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
	vy = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
	vz = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
	vw = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
	vx = _mm_mul_ps(vx, m2.r[0]);
	vy = _mm_mul_ps(vy, m2.r[1]);
	vz = _mm_mul_ps(vz, m2.r[2]);
	vw = _mm_mul_ps(vw, m2.r[3]);
	vx = _mm_add_ps(vx, vz);
	vy = _mm_add_ps(vy, vw);
	vx = _mm_add_ps(vx, vy);
	result.r[2] = vx;
	vw = m1.r[3];
	vx = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
	vy = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
	vz = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
	vw = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
	vx = _mm_mul_ps(vx, m2.r[0]);
	vy = _mm_mul_ps(vy, m2.r[1]);
	vz = _mm_mul_ps(vz, m2.r[2]);
	vw = _mm_mul_ps(vw, m2.r[3]);
	vx = _mm_add_ps(vx, vz);
	vy = _mm_add_ps(vy, vw);
	vx = _mm_add_ps(vx, vy);
	result.r[3] = vx;
	return result;
#else
	Matrix4x4 res;
	for (int i = 0; i < 4; ++i) {
		float x = m1.m[i * 4 + 0];
		float y = m1.m[i * 4 + 1];
		float z = m1.m[i * 4 + 2];
		float w = m1.m[i * 4 + 3];
		res.m[i * 4 + 0] = (m2.m[0 * 4 + 0] * x) + (m2.m[1 * 4 + 0] * y)
			+ (m2.m[2 * 4 + 0] * z) + (m2.m[3 * 4 + 0] * w);
		res.m[i * 4 + 1] = (m2.m[0 * 4 + 1] * x) + (m2.m[1 * 4 + 1] * y)
			+ (m2.m[2 * 4 + 1] * z) + (m2.m[3 * 4 + 1] * w);
		res.m[i * 4 + 2] = (m2.m[0 * 4 + 2] * x) + (m2.m[1 * 4 + 2] * y)
			+ (m2.m[2 * 4 + 2] * z) + (m2.m[3 * 4 + 2] * w);
		res.m[i * 4 + 3] = (m2.m[0 * 4 + 3] * x) + (m2.m[1 * 4 + 3] * y)
			+ (m2.m[2 * 4 + 3] * z) + (m2.m[3 * 4 + 3] * w);
	}

	return res;
#endif
}

inline void MatOp<_Matrix4x4>::TransformOrtho(pref m, const TraitsVec3::type * inpstream,
	uint32 inpstride, uint32 count, TraitsVec4::type * outstream, uint32 outstride) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	NEX_ASSERT(outstream);
	NEX_ASSERT(inpstream);
	const uint8* inpVec = (const uint8*)inpstream;
	uint8* outVec = (uint8*)outstream;

	for (uint32 i = 0; i < count; i++) {
		_Quad x = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->x);
		_Quad y = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->y);
		_Quad res = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->z);
		res = _mm_mul_ps(res, m.r[2]);
		res = _mm_add_ps(res, m.r[3]);
		y = _mm_mul_ps(y, m.r[1]);
		res = _mm_add_ps(res, y);
		x = _mm_mul_ps(x, m.r[0]);
		res = _mm_add_ps(res, x);
		_mm_store_ps(reinterpret_cast<float *>(outVec), res);
		inpVec += inpstride;
		outVec += outstride;
	}
#else
	NEX_ASSERT(outstream);
	NEX_ASSERT(inpstream);
	const uint8* inpVec = (const uint8*)inpstream;
	uint8* outVec = (uint8*)outstream;

	_Quad  x, y, z, r;

	for (uint32 i = 0; i < count; i++) {

		
		x = Vec3Op::SplatX(((float*)inpVec)[0]);
		y = Vec3Op::SplatY(((float*)inpVec)[1]);
		z = Vec3Op::SplatZ(((float*)inpVec)[2]);

		r = Vec3AOp::Madd(z, Row(m, 2), Row(m, 3));
		r = Vec3AOp::Madd(y, Row(m, 1), r);
		r = Vec3AOp::Madd(x, Row(m, 0), r);

		((float*)outVec)[0] = r.x;
		((float*)outVec)[1] = r.y;
		((float*)outVec)[2] = r.z;
		((float*)outVec)[3] = r.w;
		
		inpVec += inpstride;
		outVec += outstride;
	}
#endif
}

inline void MatOp<_Matrix4x4>::TransformOrtho(pref m, 
	TraitsVec3::type * ioStream, uint32 inStride, uint32 count) {
	union {
		_Quad v;
		float_type s[4];
	}store;

#if NEX_VECTOR_MATH_TYPE_IS_SSE
	NEX_ASSERT(ioStream);
	uint8* inpVec = (uint8*)ioStream;
	
	for (uint32 i = 0; i < count; i++) {
		_Quad x = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->x);
		_Quad y = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->y);
		_Quad res = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->z);
		res = _mm_mul_ps(res, m.r[2]);
		res = _mm_add_ps(res, m.r[3]);
		y = _mm_mul_ps(y, m.r[1]);
		res = _mm_add_ps(res, y);
		x = _mm_mul_ps(x, m.r[0]);
		res = _mm_add_ps(res, x);
		_mm_store_ps(store.s, res);
		((float*)inpVec)[0] = store.s[0];
		((float*)inpVec)[1] = store.s[1];
		((float*)inpVec)[2] = store.s[2];

		inpVec += inStride;
	}
#else
	NEX_ASSERT(outstream);
	NEX_ASSERT(inpstream);
	uint8* inpVec = (uint8*)inpstream;

	_Quad  x, y, z, r;

	for (uint32 i = 0; i < count; i++) {

		x = Vec3Op::SplatX(((float*)inpVec)[0]);
		y = Vec3Op::SplatY(((float*)inpVec)[1]);
		z = Vec3Op::SplatZ(((float*)inpVec)[2]);

		r = Vec3AOp::Madd(z, Row(m, 2), Row(m, 3));
		r = Vec3AOp::Madd(y, Row(m, 1), r);
		r = Vec3AOp::Madd(x, Row(m, 0), r);

		((float*)inpVec)[0] = r.x;
		((float*)inpVec)[1] = r.y;
		((float*)inpVec)[2] = r.z;

		inpVec += inStride;
	}
#endif
}

inline void MatOp<_Matrix4x4>::Transform(pref m, 
	const TraitsVec3::type *inpstream, uint32 inpstride, uint32 count,
	TraitsVec3::type * outstream, uint32 outstride) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	NEX_ASSERT(outstream);
	NEX_ASSERT(inpstream);
	const uint8 *inpVec = (uint8*)inpstream;
	uint8 *outVec = (uint8*)outstream;

	for (uint32 i = 0; i < count; i++) {
		_Quad x = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->x);
		_Quad y = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->y);
		_Quad res = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->z);
		res = _mm_mul_ps(res, m.r[2]);
		res = _mm_add_ps(res, m.r[3]);
		y = _mm_mul_ps(y, m.r[1]);
		res = _mm_add_ps(res, y);
		x = _mm_mul_ps(x, m.r[0]);
		res = _mm_add_ps(res, x);

		x = _mm_shuffle_ps(res, res, _MM_SHUFFLE(3, 3, 3, 3));
		res = _mm_div_ps(res, x);
		_mm_store_ss(&reinterpret_cast<Vector3*>(outVec)->x, res);
		res = _mm_shuffle_ps(res, res, _MM_SHUFFLE(0, 3, 2, 1));
		_mm_store_ss(&reinterpret_cast<Vector3*>(outVec)->y, res);
		res = _mm_shuffle_ps(res, res, _MM_SHUFFLE(0, 3, 2, 1));
		_mm_store_ss(&reinterpret_cast<Vector3*>(outVec)->z, res);
		inpVec += inpstride;
		outVec += outstride;
	}
#else

	NEX_ASSERT(outstream);
	NEX_ASSERT(inpstream);
	const uint8* inpVec = (const uint8*)inpstream;
	uint8* outVec = (uint8*)outstream;

	_Quad x, y, z, r;

	for (uint32 i = 0; i < count; i++) {

		x = Vec3Op::SplatX(((float*)inpVec)[0]);
		y = Vec3Op::SplatY(((float*)inpVec)[1]);
		z = Vec3Op::SplatZ(((float*)inpVec)[2]);

		r = Vec3AOp::Madd(z, Row(m, 2), Row(m, 3));
		r = Vec3AOp::Madd(y, Row(m, 1), r);
		r = Vec3AOp::Madd(x, Row(m, 0), r);
		r = Vec3AOp::Mul(r, 1.f / r.w);

		((float*)outVec)[0] = r.x;
		((float*)outVec)[1] = r.y;
		((float*)outVec)[2] = r.z;
		((float*)outVec)[3] = r.w;

		inpVec += inpstride;
		outVec += outstride;
	}
#endif
}

inline TraitsVec3A::type MatOp<_Matrix4x4>::TransformOrtho(pref m, TraitsVec3A::pref v) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	_Quad ret;
	ret = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
	ret = _mm_mul_ps(ret, m.r[0]);
	_Quad vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
	vTemp = _mm_mul_ps(vTemp, m.r[1]);
	ret = _mm_add_ps(ret, vTemp);
	vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
	vTemp = _mm_mul_ps(vTemp, m.r[2]);
	ret = _mm_add_ps(ret, vTemp);
	ret = _mm_add_ps(ret, m.r[3]);
	return ret;
#else
	_Quad r, x, y, z;

	x = Vec3Op::SplatX(v);
	y = Vec3Op::SplatY(v);
	z = Vec3Op::SplatZ(v);

	r = Vec3AOp::Madd(z, Row(m, 2), Row(m, 3));
	r = Vec3AOp::Madd(y, Row(m, 1), r);
	r = Vec3AOp::Madd(x, Row(m, 0), r);
	return r;
#endif
}

inline TraitsVec3A::type MatOp<_Matrix4x4>::Transform(pref m, TraitsVec3A::pref v) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	_Quad ret;
	ret = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
	ret = _mm_mul_ps(ret, m.r[0]);
	_Quad vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
	vTemp = _mm_mul_ps(vTemp, m.r[1]);
	ret = _mm_add_ps(ret, vTemp);
	vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
	vTemp = _mm_mul_ps(vTemp, m.r[2]);
	ret = _mm_add_ps(ret, vTemp);
	ret = _mm_add_ps(ret, m.r[3]);
	vTemp = _mm_shuffle_ps(ret, ret, _MM_SHUFFLE(3, 3, 3, 3));
	ret = _mm_div_ps(ret, vTemp);
	return ret;
#else
	_Quad r, x, y, z;

	x = Vec3Op::SplatX(v);
	y = Vec3Op::SplatY(v);
	z = Vec3Op::SplatZ(v);

	r = Vec3AOp::Madd(z, Row(m, 2), Row(m, 3));
	r = Vec3AOp::Madd(y, Row(m, 1), r);
	r = Vec3AOp::Madd(x, Row(m, 0), r);
	r = Vec3AOp::Mul(r, 1.f / r.w);

	return r;
#endif
}

inline TraitsVec3A::type MatOp<_Matrix4x4>::TransformBounds(pref m, TraitsVec3A::pref v) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	_Quad ret = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
	_Quad clearSign = N3D_ClearSign.v;
	ret = _mm_mul_ps(ret, m.r[0]);
	ret = _mm_and_ps(ret, clearSign);
	_Quad vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
	vTemp = _mm_mul_ps(vTemp, m.r[1]);
	vTemp = _mm_and_ps(vTemp, clearSign);
	ret = _mm_add_ps(ret, vTemp);
	vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
	vTemp = _mm_mul_ps(vTemp, m.r[2]);
	vTemp = _mm_and_ps(vTemp, clearSign);
	ret = _mm_add_ps(ret, vTemp);
	return ret; 
#else
	Vector3A ret;
	for (int i = 0; i < 3; i++) {
		ret.v[i] = 0.0f;
		for (int j = 0; j < 3; j++) {
			ret.v[i] += v.v[j] * Math::Abs(m.m[i + j * 4]);
		}
		}
	return ret; 
#endif
}

inline TraitsAABox::type MatOp<_Matrix4x4>::TransformAABox(pref m, TraitsAABox::pref box) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	AxisAlignedBox ret;
	_Quad max0 = _mm_shuffle_ps(box.maxPoint, box.maxPoint, _MM_SHUFFLE(0, 0, 0, 0));
	max0 = _mm_mul_ps(max0, m.r[0]);
	_Quad min0 = _mm_shuffle_ps(box.minPoint, box.minPoint, _MM_SHUFFLE(0, 0, 0, 0));
	min0 = _mm_mul_ps(min0, m.r[0]);
	_Quad max1 = _mm_shuffle_ps(box.maxPoint, box.maxPoint, _MM_SHUFFLE(1, 1, 1, 1));
	max1 = _mm_mul_ps(max1, m.r[1]);
	_Quad min1 = _mm_shuffle_ps(box.minPoint, box.minPoint, _MM_SHUFFLE(1, 1, 1, 1));
	min1 = _mm_mul_ps(min1, m.r[1]);
	_Quad max2 = _mm_shuffle_ps(box.maxPoint, box.maxPoint, _MM_SHUFFLE(2, 2, 2, 2));
	max2 = _mm_mul_ps(max2, m.r[2]);
	_Quad min2 = _mm_shuffle_ps(box.minPoint, box.minPoint, _MM_SHUFFLE(2, 2, 2, 2));
	min2 = _mm_mul_ps(min2, m.r[2]);

	ret.minPoint = _mm_add_ps(_mm_add_ps(_mm_min_ps(max0, min0), _mm_add_ps(_mm_min_ps(max1, min1), _mm_min_ps(max2, min2))), m.r[3]);
	ret.maxPoint = _mm_add_ps(_mm_add_ps(_mm_max_ps(max0, min0), _mm_add_ps(_mm_max_ps(max1, min1), _mm_max_ps(max2, min2))), m.r[3]);
	return ret; 
#else
	AxisAlignedBox ret;
	for (int i = 0; i < 3; i++) {
		ret.minPoint.v[i] = std::min(box.minPoint.v[0] * m.m[i + 0 * 4], box.maxPoint.v[0] * m.m[i + 0 * 4]) +
			std::min(box.minPoint.v[1] * m.m[i + 1 * 4], box.maxPoint.v[1] * m.m[i + 1 * 4]) +
			std::min(box.minPoint.v[2] * m.m[i + 2 * 4], box.maxPoint.v[2] * m.m[i + 2 * 4]) +
			m.m[i + 3 * 4];
	}
	ret.minPoint.v[3] = 0;
	for (int i = 0; i < 3; i++) {
		ret.maxPoint.v[i] = std::max(box.minPoint.v[0] * m.m[i + 0 * 4], box.maxPoint.v[0] * m.m[i + 0 * 4]) +
			std::max(box.minPoint.v[1] * m.m[i + 1 * 4], box.maxPoint.v[1] * m.m[i + 1 * 4]) +
			std::max(box.minPoint.v[2] * m.m[i + 2 * 4], box.maxPoint.v[2] * m.m[i + 2 * 4]) +
			m.m[i + 3 * 4];
	}
	ret.maxPoint.v[3] = 0;
	return ret;

#endif
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::FromScaleRotPos(float_type scale, 
	TraitsQuat::pref rot, TraitsVec3A::pref pos) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	_Quad r0, r1, r2;
	_Quad q0, q1;
	_Quad v0, v1, v2;

	q0 = _mm_add_ps(rot, rot);
	q1 = _mm_mul_ps(rot, q0);

	v0 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 0, 0, 1));
	v0 = _mm_and_ps(v0, N3D_FFFO.v);
	v1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 1, 2, 2));
	v1 = _mm_and_ps(v1, N3D_FFFO.v);
	r0 = _mm_sub_ps(N3D_1110.v, v0);
	r0 = _mm_sub_ps(r0, v1);

	v0 = _mm_shuffle_ps(rot, rot, _MM_SHUFFLE(3, 1, 0, 0));
	v1 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3, 2, 1, 2));
	v0 = _mm_mul_ps(v0, v1);

	v1 = _mm_shuffle_ps(rot, rot, _MM_SHUFFLE(3, 3, 3, 3));
	v2 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3, 0, 2, 1));
	v1 = _mm_mul_ps(v1, v2);

	r1 = _mm_add_ps(v0, v1);
	r2 = _mm_sub_ps(v0, v1);

	v0 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(1, 0, 2, 1));
	v0 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(1, 3, 2, 0));
	v1 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(2, 2, 0, 0));
	v1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(2, 0, 2, 0));

	q1 = _mm_shuffle_ps(r0, v0, _MM_SHUFFLE(1, 0, 3, 0));
	q1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 3, 2, 0));

	_Quad scaleQ = QuadOp::Set(scale);
	Matrix4x4 ret;
	ret.r[0] = _mm_mul_ps(scaleQ, q1);
	q1 = _mm_shuffle_ps(r0, v0, _MM_SHUFFLE(3, 2, 3, 1));
	q1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 3, 0, 2));
	ret.r[1] = _mm_mul_ps(scaleQ, q1);
	q1 = _mm_shuffle_ps(v1, r0, _MM_SHUFFLE(3, 2, 1, 0));
	ret.r[2] = _mm_mul_ps(scaleQ, q1);
	ret.r[3] = _mm_or_ps(_mm_and_ps(pos, N3D_FFFO.v), N3D_0001.v);
	return ret;
#else
	Matrix4x4 ret;
	float xx = rot.x * rot.x;
	float yy = rot.y * rot.y;
	float zz = rot.z * rot.z;
	float xy = rot.x * rot.y;
	float xz = rot.x * rot.z;
	float yz = rot.y * rot.z;
	float wx = rot.w * rot.x;
	float wy = rot.w * rot.y;
	float wz = rot.w * rot.z;

	ret.m00 = scale * (1 - 2 * (yy + zz));
	ret.m01 = scale * (2 * (xy + wz));
	ret.m02 = scale * (2 * (xz - wy));

	ret.m10 = scale * (2 * (xy - wz));
	ret.m11 = scale * (1 - 2 * (xx + zz));
	ret.m12 = scale * (2 * (yz + wx));

	ret.m20 = scale * (2 * (xz + wy));
	ret.m21 = scale * (2 * (yz - wx));
	ret.m22 = scale * (1 - 2 * (xx + yy));

	ret.m03 = ret.m13 = ret.m23 = 0.0f;
	ret.m30 = pos.x;
	ret.m31 = pos.y;
	ret.m32 = pos.z;
	ret.m33 = 1.0f;

	return ret;
#endif
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::FromScale(TraitsVec3A::pref scale) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	Matrix4x4 ret;
	ret.r[0] = _mm_and_ps(scale, N3D_F00O.v);
	ret.r[1] = _mm_and_ps(scale, N3D_0F0O.v);
	ret.r[2] = _mm_and_ps(scale, N3D_00FO.v);
	ret.r[3] = N3D_0001.v;
	return ret;
#else
	Matrix4x4 ret;
	ret.m00 = scale.x; ret.m01 = 0; ret.m02 = 0; ret.m03 = 0;
	ret.m10 = 0; ret.m11 = scale.y; ret.m12 = 0; ret.m13 = 0;
	ret.m20 = 0; ret.m21 = 0; ret.m22 = scale.z; ret.m23 = 0;
	ret.m30 = 0; ret.m31 = 0; ret.m32 = 0; ret.m33 = 1;
	return ret;
#endif
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::FromPos(TraitsVec3A::pref pos) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	Matrix4x4 ret;
	ret.r[0] = N3D_1000.v;
	ret.r[1] = N3D_0100.v;
	ret.r[2] = N3D_0010.v;
	ret.r[3] = _mm_or_ps(_mm_and_ps(pos, N3D_FFFO.v), N3D_0001.v);
	return ret;
#else
	Matrix4x4 ret;
	ret.m00 = 1; ret.m01 = 0; ret.m02 = 0; ret.m03 = 0;
	ret.m10 = 0; ret.m11 = 1; ret.m12 = 0; ret.m13 = 0;
	ret.m20 = 0; ret.m21 = 0; ret.m22 = 1; ret.m23 = 0;
	ret.m30 = pos.x; ret.m31 = pos.y; ret.m32 = pos.z; ret.m33 = 1;
	return ret;
#endif
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::FromQuat(TraitsQuat::pref rot) {
	type ret;
	SetRot(ret, rot);

#if NEX_VECTOR_MATH_TYPE_IS_SSE
	ret.r[3] = N3D_0001.v;
#else
	ret.m30 = 0;
	ret.m31 = 0;
	ret.m32 = 0;
	ret.m33 = 1.0f;
#endif
	return ret;
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::FromRot(TraitsQuat::pref rot) {
	return FromQuat(rot);
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::FromWorldToView(pref m) {
	return InverseOrtho(m);
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::FromLookAt(TraitsVec3A::pref eye, 
	TraitsVec3A::pref lookAt, TraitsVec3A::pref vup) {
	Matrix4x4 m;
	Vec3A::type zaxis = m.r[2] = Vec3AOp::Normalize(Vec3AOp::Sub(lookAt, eye));
	Vec3A::type xaxis = m.r[0] = Vec3AOp::Normalize(Vec3AOp::Cross(vup, m.r[2]));
	Vec3A::type yaxis = m.r[1] = Vec3AOp::Cross(m.r[2], m.r[0]);
	m.r[3] = Matrix4x4::IdentityMatrix.r[3];
	m = Transpose(m);

	Vector3A negEye = Vec3AOp::Negate(eye);
	m.r[3] = Vec4::Set(
		Vec3AOp::Dot(xaxis, negEye),
		Vec3AOp::Dot(yaxis, negEye),
		Vec3AOp::Dot(zaxis, negEye),
		1);

	return m;
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::FromOrthoProjection(float_type w, float_type h,
	float_type zn, float_type zf) {
	float dzRecip = 1.0f / (zf - zn);
	return { 2 / w, 0, 0, 0, 0, 2 / h, 0, 0, 0, 0, -2 * dzRecip, 0, 0,
		0, -(zn + zf) * dzRecip, 1
	};
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::FromPerspectiveProjection(float_type fieldOfView, float_type aspectRatio, 
	float_type zn, float_type zf) {

	fieldOfView *= 0.5f;

	float yscale = 1 / Math::Tan(fieldOfView);
	float q = zf / (zf - zn);
	return { (yscale / aspectRatio), 0, 0, 0, 0, yscale, 0, 0, 0, 0, q,
		1, 0, 0, -q * zn, 0 };
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::Mul(pref m, float_type scale) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	Matrix4x4 ret;
	_Quad scaleQ = QuadOp::Set(scale);
	ret.r[0] = _mm_mul_ps(scaleQ, m.r[0]);
	ret.r[1] = _mm_mul_ps(scaleQ, m.r[1]);
	ret.r[2] = _mm_mul_ps(scaleQ, m.r[2]);
	ret.r[3] = m.r[3];
	return ret;
#else
	Matrix4x4 ret = m;
	ret.m00 *= scale;
	ret.m01 *= scale;
	ret.m02 *= scale;

	ret.m10 *= scale;
	ret.m11 *= scale;
	ret.m12 *= scale;

	ret.m20 *= scale;
	ret.m21 *= scale;
	ret.m22 *= scale;
	return ret;
#endif
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::Mul(float_type scale, pref m) {
	return Mul(m, scale);
}

inline TraitsVec4::type MatOp<_Matrix4x4>::Mul(TraitsVec4::pref v, pref m) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	_Quad ret, vTemp;
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
	_Quad r, x, y, z, w;

	x = Vec3Op::SplatX(v);
	y = Vec3Op::SplatY(v);
	z = Vec3Op::SplatZ(v);
	w = Vec3Op::SplatW(v);

	r = Vec3AOp::Mul(w, Row(m, 3));
	r = Vec3AOp::Madd(z, Row(m, 2), r);
	r = Vec3AOp::Madd(y, Row(m, 1), r);
	r = Vec3AOp::Madd(x, Row(m, 0), r);

	return r;
#endif
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::Transpose(pref m) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	// x.x,x.y,y.x,y.y
	_Quad temp1 = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(1, 0, 1, 0));
	// x.z,x.w,y.z,y.w
	_Quad temp3 = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(3, 2, 3, 2));
	// z.x,z.y,w.x,w.y
	_Quad temp2 = _mm_shuffle_ps(m.r[2], m.r[3], _MM_SHUFFLE(1, 0, 1, 0));
	// z.z,z.w,w.z,w.w
	_Quad temp4 = _mm_shuffle_ps(m.r[2], m.r[3], _MM_SHUFFLE(3, 2, 3, 2));
	Matrix4x4 result;

	// x.x,y.x,z.x,w.x
	result.r[0] = _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(2, 0, 2, 0));
	// x.y,y.y,z.y,w.y
	result.r[1] = _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(3, 1, 3, 1));
	// x.z,y.z,z.z,w.z
	result.r[2] = _mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(2, 0, 2, 0));
	// x.w,y.w,z.w,w.w
	result.r[3] = _mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(3, 1, 3, 1));

	return result;
#else
	Matrix4x4 ret;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			ret.m[i * 4 + j] = m.m[j * 4 + i];

	return ret; 
#endif
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::Inverse(pref m) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	Matrix4x4 MT = Transpose(m);
	_Quad V00 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(1, 1, 0, 0));
	_Quad V10 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(3, 2, 3, 2));
	_Quad V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(1, 1, 0, 0));
	_Quad V11 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(3, 2, 3, 2));
	_Quad V02 = _mm_shuffle_ps(MT.r[2], MT.r[0], _MM_SHUFFLE(2, 0, 2, 0));
	_Quad V12 = _mm_shuffle_ps(MT.r[3], MT.r[1], _MM_SHUFFLE(3, 1, 3, 1));

	_Quad D0 = _mm_mul_ps(V00, V10);
	_Quad D1 = _mm_mul_ps(V01, V11);
	_Quad D2 = _mm_mul_ps(V02, V12);

	V00 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(3, 2, 3, 2));
	V10 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(1, 1, 0, 0));
	V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(3, 2, 3, 2));
	V11 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(1, 1, 0, 0));
	V02 = _mm_shuffle_ps(MT.r[2], MT.r[0], _MM_SHUFFLE(3, 1, 3, 1));
	V12 = _mm_shuffle_ps(MT.r[3], MT.r[1], _MM_SHUFFLE(2, 0, 2, 0));

	V00 = _mm_mul_ps(V00, V10);
	V01 = _mm_mul_ps(V01, V11);
	V02 = _mm_mul_ps(V02, V12);
	D0 = _mm_sub_ps(D0, V00);
	D1 = _mm_sub_ps(D1, V01);
	D2 = _mm_sub_ps(D2, V02);
	// V11 = D0Y,D0W,D2Y,D2Y
	V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 1, 3, 1));
	V00 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(1, 0, 2, 1));
	V10 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(0, 3, 0, 2));
	V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(0, 1, 0, 2));
	V11 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(2, 1, 2, 1));
	// V13 = D1Y,D1W,D2W,D2W
	_Quad V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 3, 3, 1));
	V02 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(1, 0, 2, 1));
	V12 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(0, 3, 0, 2));
	_Quad V03 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(0, 1, 0, 2));
	V13 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(2, 1, 2, 1));

	_Quad C0 = _mm_mul_ps(V00, V10);
	_Quad C2 = _mm_mul_ps(V01, V11);
	_Quad C4 = _mm_mul_ps(V02, V12);
	_Quad C6 = _mm_mul_ps(V03, V13);

	// V11 = D0X,D0Y,D2X,D2X
	V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(0, 0, 1, 0));
	V00 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(2, 1, 3, 2));
	V10 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(2, 1, 0, 3));
	V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(1, 3, 2, 3));
	V11 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(0, 2, 1, 2));
	// V13 = D1X,D1Y,D2Z,D2Z
	V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(2, 2, 1, 0));
	V02 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(2, 1, 3, 2));
	V12 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(2, 1, 0, 3));
	V03 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(1, 3, 2, 3));
	V13 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(0, 2, 1, 2));

	V00 = _mm_mul_ps(V00, V10);
	V01 = _mm_mul_ps(V01, V11);
	V02 = _mm_mul_ps(V02, V12);
	V03 = _mm_mul_ps(V03, V13);
	C0 = _mm_sub_ps(C0, V00);
	C2 = _mm_sub_ps(C2, V01);
	C4 = _mm_sub_ps(C4, V02);
	C6 = _mm_sub_ps(C6, V03);

	V00 = _mm_shuffle_ps(MT.r[1], MT.r[1], _MM_SHUFFLE(0, 3, 0, 3));
	// V10 = D0Z,D0Z,D2X,D2Y
	V10 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 2, 2));
	V10 = _mm_shuffle_ps(V10, V10, _MM_SHUFFLE(0, 2, 3, 0));
	V01 = _mm_shuffle_ps(MT.r[0], MT.r[0], _MM_SHUFFLE(2, 0, 3, 1));
	// V11 = D0X,D0W,D2X,D2Y
	V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 3, 0));
	V11 = _mm_shuffle_ps(V11, V11, _MM_SHUFFLE(2, 1, 0, 3));
	V02 = _mm_shuffle_ps(MT.r[3], MT.r[3], _MM_SHUFFLE(0, 3, 0, 3));
	// V12 = D1Z,D1Z,D2Z,D2W
	V12 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 2, 2));
	V12 = _mm_shuffle_ps(V12, V12, _MM_SHUFFLE(0, 2, 3, 0));
	V03 = _mm_shuffle_ps(MT.r[2], MT.r[2], _MM_SHUFFLE(2, 0, 3, 1));
	// V13 = D1X,D1W,D2Z,D2W
	V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 3, 0));
	V13 = _mm_shuffle_ps(V13, V13, _MM_SHUFFLE(2, 1, 0, 3));

	V00 = _mm_mul_ps(V00, V10);
	V01 = _mm_mul_ps(V01, V11);
	V02 = _mm_mul_ps(V02, V12);
	V03 = _mm_mul_ps(V03, V13);
	_Quad C1 = _mm_sub_ps(C0, V00);
	C0 = _mm_add_ps(C0, V00);
	_Quad C3 = _mm_add_ps(C2, V01);
	C2 = _mm_sub_ps(C2, V01);
	_Quad C5 = _mm_sub_ps(C4, V02);
	C4 = _mm_add_ps(C4, V02);
	_Quad C7 = _mm_add_ps(C6, V03);
	C6 = _mm_sub_ps(C6, V03);

	C0 = _mm_shuffle_ps(C0, C1, _MM_SHUFFLE(3, 1, 2, 0));
	C2 = _mm_shuffle_ps(C2, C3, _MM_SHUFFLE(3, 1, 2, 0));
	C4 = _mm_shuffle_ps(C4, C5, _MM_SHUFFLE(3, 1, 2, 0));
	C6 = _mm_shuffle_ps(C6, C7, _MM_SHUFFLE(3, 1, 2, 0));
	C0 = _mm_shuffle_ps(C0, C0, _MM_SHUFFLE(3, 1, 2, 0));
	C2 = _mm_shuffle_ps(C2, C2, _MM_SHUFFLE(3, 1, 2, 0));
	C4 = _mm_shuffle_ps(C4, C4, _MM_SHUFFLE(3, 1, 2, 0));
	C6 = _mm_shuffle_ps(C6, C6, _MM_SHUFFLE(3, 1, 2, 0));
	// Get the determinate
#ifdef NEX_USE_FAST_DIVISION
	_Quad vTemp = QuadOp::SplatX(_mm_rcp_ss(QuadOp::VDot(C0, MT.r[0])));
#else
	_Quad vTemp = QuadOp::SplatX(_mm_div_ss(N3D_1000.v, QuadOp::VDot(C0, MT.r[0])));
#endif
	Matrix4x4 mResult;
	mResult.r[0] = _mm_mul_ps(C0, vTemp);
	mResult.r[1] = _mm_mul_ps(C2, vTemp);
	mResult.r[2] = _mm_mul_ps(C4, vTemp);
	mResult.r[3] = _mm_mul_ps(C6, vTemp);
	return mResult;

#else
	Matrix4x4 inv;

	inv.m[0] = m.m[5] * m.m[10] * m.m[15] -
		m.m[5] * m.m[11] * m.m[14] -
		m.m[9] * m.m[6] * m.m[15] +
		m.m[9] * m.m[7] * m.m[14] +
		m.m[13] * m.m[6] * m.m[11] -
		m.m[13] * m.m[7] * m.m[10];

	inv.m[4] = -m.m[4] * m.m[10] * m.m[15] +
		m.m[4] * m.m[11] * m.m[14] +
		m.m[8] * m.m[6] * m.m[15] -
		m.m[8] * m.m[7] * m.m[14] -
		m.m[12] * m.m[6] * m.m[11] +
		m.m[12] * m.m[7] * m.m[10];

	inv.m[8] = m.m[4] * m.m[9] * m.m[15] -
		m.m[4] * m.m[11] * m.m[13] -
		m.m[8] * m.m[5] * m.m[15] +
		m.m[8] * m.m[7] * m.m[13] +
		m.m[12] * m.m[5] * m.m[11] -
		m.m[12] * m.m[7] * m.m[9];

	inv.m[12] = -m.m[4] * m.m[9] * m.m[14] +
		m.m[4] * m.m[10] * m.m[13] +
		m.m[8] * m.m[5] * m.m[14] -
		m.m[8] * m.m[6] * m.m[13] -
		m.m[12] * m.m[5] * m.m[10] +
		m.m[12] * m.m[6] * m.m[9];

	inv.m[1] = -m.m[1] * m.m[10] * m.m[15] +
		m.m[1] * m.m[11] * m.m[14] +
		m.m[9] * m.m[2] * m.m[15] -
		m.m[9] * m.m[3] * m.m[14] -
		m.m[13] * m.m[2] * m.m[11] +
		m.m[13] * m.m[3] * m.m[10];

	inv.m[5] = m.m[0] * m.m[10] * m.m[15] -
		m.m[0] * m.m[11] * m.m[14] -
		m.m[8] * m.m[2] * m.m[15] +
		m.m[8] * m.m[3] * m.m[14] +
		m.m[12] * m.m[2] * m.m[11] -
		m.m[12] * m.m[3] * m.m[10];

	inv.m[9] = -m.m[0] * m.m[9] * m.m[15] +
		m.m[0] * m.m[11] * m.m[13] +
		m.m[8] * m.m[1] * m.m[15] -
		m.m[8] * m.m[3] * m.m[13] -
		m.m[12] * m.m[1] * m.m[11] +
		m.m[12] * m.m[3] * m.m[9];

	inv.m[13] = m.m[0] * m.m[9] * m.m[14] -
		m.m[0] * m.m[10] * m.m[13] -
		m.m[8] * m.m[1] * m.m[14] +
		m.m[8] * m.m[2] * m.m[13] +
		m.m[12] * m.m[1] * m.m[10] -
		m.m[12] * m.m[2] * m.m[9];

	inv.m[2] = m.m[1] * m.m[6] * m.m[15] -
		m.m[1] * m.m[7] * m.m[14] -
		m.m[5] * m.m[2] * m.m[15] +
		m.m[5] * m.m[3] * m.m[14] +
		m.m[13] * m.m[2] * m.m[7] -
		m.m[13] * m.m[3] * m.m[6];

	inv.m[6] = -m.m[0] * m.m[6] * m.m[15] +
		m.m[0] * m.m[7] * m.m[14] +
		m.m[4] * m.m[2] * m.m[15] -
		m.m[4] * m.m[3] * m.m[14] -
		m.m[12] * m.m[2] * m.m[7] +
		m.m[12] * m.m[3] * m.m[6];

	inv.m[10] = m.m[0] * m.m[5] * m.m[15] -
		m.m[0] * m.m[7] * m.m[13] -
		m.m[4] * m.m[1] * m.m[15] +
		m.m[4] * m.m[3] * m.m[13] +
		m.m[12] * m.m[1] * m.m[7] -
		m.m[12] * m.m[3] * m.m[5];

	inv.m[14] = -m.m[0] * m.m[5] * m.m[14] +
		m.m[0] * m.m[6] * m.m[13] +
		m.m[4] * m.m[1] * m.m[14] -
		m.m[4] * m.m[2] * m.m[13] -
		m.m[12] * m.m[1] * m.m[6] +
		m.m[12] * m.m[2] * m.m[5];

	inv.m[3] = -m.m[1] * m.m[6] * m.m[11] +
		m.m[1] * m.m[7] * m.m[10] +
		m.m[5] * m.m[2] * m.m[11] -
		m.m[5] * m.m[3] * m.m[10] -
		m.m[9] * m.m[2] * m.m[7] +
		m.m[9] * m.m[3] * m.m[6];

	inv.m[7] = m.m[0] * m.m[6] * m.m[11] -
		m.m[0] * m.m[7] * m.m[10] -
		m.m[4] * m.m[2] * m.m[11] +
		m.m[4] * m.m[3] * m.m[10] +
		m.m[8] * m.m[2] * m.m[7] -
		m.m[8] * m.m[3] * m.m[6];

	inv.m[11] = -m.m[0] * m.m[5] * m.m[11] +
		m.m[0] * m.m[7] * m.m[9] +
		m.m[4] * m.m[1] * m.m[11] -
		m.m[4] * m.m[3] * m.m[9] -
		m.m[8] * m.m[1] * m.m[7] +
		m.m[8] * m.m[3] * m.m[5];

	inv.m[15] = m.m[0] * m.m[5] * m.m[10] -
		m.m[0] * m.m[6] * m.m[9] -
		m.m[4] * m.m[1] * m.m[10] +
		m.m[4] * m.m[2] * m.m[9] +
		m.m[8] * m.m[1] * m.m[6] -
		m.m[8] * m.m[2] * m.m[5];

	float det = m.m[0] * inv.m[0] + m.m[1] * inv.m[4] + m.m[2] * inv.m[8] + m.m[3] * inv.m[12];

	if (det == 0)
		return inv;

	det = 1.0f / det;

	for (int i = 0; i < 16; i++)
		inv.m[i] = inv.m[i] * det;
	return inv;
#endif
}

inline MatOp<_Matrix4x4>::type MatOp<_Matrix4x4>::InverseOrtho(pref m) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	// Inverse = [ T(R)       0 ]
	//        [ -vpos*T(R) 1 ]
	Matrix4x4 ret;
	ret.r[0] = _mm_shuffle_ps(m.r[1], m.r[2], _MM_SHUFFLE(3, 0, 0, 3));
	ret.r[0] = _mm_move_ss(ret.r[0], m.r[0]);
	ret.r[1] = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(3, 1, 3, 1));
	ret.r[1] = _mm_shuffle_ps(ret.r[1], m.r[2], _MM_SHUFFLE(3, 1, 2, 0));
	ret.r[2] = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(3, 2, 3, 2));
	ret.r[2] = _mm_shuffle_ps(ret.r[2], m.r[2], _MM_SHUFFLE(3, 2, 2, 0));

	_Quad vTemp = _mm_shuffle_ps(m.r[3], m.r[3], _MM_SHUFFLE(0, 0, 0, 0));
	ret.r[3] = _mm_mul_ps(vTemp, ret.r[0]);
	vTemp = _mm_shuffle_ps(m.r[3], m.r[3], _MM_SHUFFLE(1, 1, 1, 1));
	vTemp = _mm_mul_ps(vTemp, ret.r[1]);
	ret.r[3] = _mm_add_ps(ret.r[3], vTemp);
	vTemp = _mm_shuffle_ps(m.r[3], m.r[3], _MM_SHUFFLE(2, 2, 2, 2));
	vTemp = _mm_mul_ps(vTemp, ret.r[2]);
	ret.r[3] = _mm_add_ps(ret.r[3], vTemp);
	ret.r[3] = _mm_xor_ps(ret.r[3], N3D_FlipSign.v);
	// and with 0001
	ret.r[3] = _mm_or_ps(_mm_and_ps(ret.r[3], N3D_FFFO.v), N3D_0001.v);

	NEX_ASSERT(
		ret.m03 == 0.f && ret.m13 == 0.f && ret.m23 == 0.f
		&& ret.m33 == 1.f);
	return ret; 
#else
	Matrix4x4 ret;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j)
			ret.m[i * 4 + j] = m.m[j * 4 + i];
		ret.m[i * 4 + 3] = 0;
	}

	ret.m30 = -Vec3A::Dot(m.Row(3), m.Row(0));
	ret.m31 = -Vec3A::Dot(m.Row(3), m.Row(1));
	ret.m32 = -Vec3A::Dot(m.Row(3), m.Row(2));
	ret.m33 = 1;
	return ret;

#endif
}


}
}
