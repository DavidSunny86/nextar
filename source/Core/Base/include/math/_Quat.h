#include "VectorMethods.h"
#pragma once
using namespace nextar;
using namespace nextar::Math;

inline _VecOp<_Quat, _Quat>::type _VecOp<_Quat, _Quat>::Identity() {
	return Set(0, 0, 0, 1);
}

inline _VecOp<_Quat, _Quat>::type _VecOp<_Quat, _Quat>::FromAxisAngle(TraitsVec3::pref axis, float_type ang) {
#if NEX_CORE_DEBUG_CHECKS == 1
	float len = Vec3Op::Length(axis);
	NEX_ASSERT(NEX_FLOAT_TOLERANCE_EQUAL(len, 1, Math::EPSILON_MED));
#endif
	float f, c;
	Math::SinCos(ang * .5f, f, c);
	return Set(f * axis.x, f * axis.y, f * axis.z, c);
}

inline _VecOp<_Quat, _Quat>::type _VecOp<_Quat, _Quat>::FromAxisAngle(TraitsAxisAngle::pref axis) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
#if NEX_CORE_DEBUG_CHECKS == 1
	float len = 1; //\todo Find length of axis
	NEX_ASSERT(NEX_FLOAT_TOLERANCE_EQUAL(len, 1, Math::EPSILON_MED));
#endif
	Quad N = _mm_and_ps(axis, N3D_FFFO.v);
	N = _mm_or_ps(N, N3D_0001.v);

	float f, c;
	Math::SinCos(QuatOp::GetW(axis) * .5f, f, c);
	Quad vSineCosine = QuadOp::Set(f, f, f, c);
	return _mm_mul_ps(N, vSineCosine);
#else
	float s, c;
	Math::SinCos(ang * .5f, s, c);
	return Set(s * axis.x, s * axis.y, s * axis.z, c);
#endif
}

inline _VecOp<_Quat, _Quat>::type _VecOp<_Quat, _Quat>::FromMat4x4(TraitsMat4x4::pref m) {
	return FromMat3x4(*reinterpret_cast<const TraitsMat3x4::type*>(&m));
}

inline _VecOp<_Quat, _Quat>::type _VecOp<_Quat, _Quat>::FromMat3x4(TraitsMat3x4::pref m) {
	int maxi;
	float maxdiag, trace;
	trace = m.m00 + m.m11 + m.m22 + 1.0f;
	if (trace > 0.0f) {
		return Set((m.m12 - m.m21) / (2.0f * sqrt(trace)),
			(m.m20 - m.m02) / (2.0f * sqrt(trace)),
			(m.m01 - m.m10) / (2.0f * sqrt(trace)), sqrt(trace) / 2.0f);
	}
	maxi = 0;
	maxdiag = m.m00;

	if (m.m11 > maxdiag) {
		maxdiag = m.m11;
		maxi = 1;
	}

	if (m.m22 > maxdiag) {
		maxdiag = m.m22;
		maxi = 2;
	}

	float s, invS;
	switch (maxi) {
	case 0:
	s = 2.0f * sqrt(1.0f + m.m00 - m.m11 - m.m22);
	invS = 1 / s;
	return Set(0.25f * s, (m.m01 + m.m10) * invS,
		(m.m02 + m.m20) * invS, (m.m12 - m.m21) * invS);

	case 1:
	s = 2.0f * sqrt(1.0f + m.m11 - m.m00 - m.m22);
	invS = 1 / s;
	return Set((m.m01 + m.m10) * invS, 0.25f * s,
		(m.m12 + m.m21) * invS, (m.m20 - m.m02) * invS);
	case 2:
	default:
	s = 2.0f * sqrt(1.0f + m.m22 - m.m00 - m.m11);
	invS = 1 / s;
	return Set((m.m02 + m.m20) * invS, (m.m12 + m.m21) * invS,
		0.25f * s, (m.m01 - m.m10) * invS);
	}
}

inline TraitsVec3A::type _VecOp<_Quat, _Quat>::Transform(pref q, TraitsVec3A::pref v) {
	TraitsVec3A::type uv = Vec3AOp::Cross(q, v);
	TraitsVec3A::type uuv = Vec3AOp::Cross(q, uv);
	return Vec3AOp::Add(
		Vec3AOp::Add(v, QuadOp::Mul(uv, 2 * QuatOp::GetW(q))),
		Vec3AOp::Add(uuv, uuv));
}

inline TraitsVec3A::type _VecOp<_Quat, _Quat>::TransformBounds(pref q, TraitsVec3A::pref v) {
	Vector3A uv = Vec3AOp::Cross(q, v);
	Vector3A uuv = Vec3AOp::Cross(q, uv);
	return Vec3AOp::Add(Vec3AOp::Add(v, Vec3AOp::Abs(QuadOp::Mul(uv, 2 * QuatOp::GetW(q)))),
		Vec3AOp::Abs(Vec3AOp::Add(uuv, uuv)));
}

inline _VecOp<_Quat, _Quat>::type _VecOp<_Quat, _Quat>::Slerp(pref from, pref to, float_type t) {
	float cosom, absCosom, sinom, omega, scale0, scale1;
	cosom = Vec4Op::Dot(from, to);
	absCosom = Math::Abs(cosom);
	if ((1.0f - absCosom) > Math::EPSILON) {
		omega = Math::ArcCos(absCosom);
		sinom = 1.0f / Math::Sin(omega);
		scale0 = Math::Sin((1.0f - t) * omega) * sinom;
		scale1 = Math::Sin(t * omega) * sinom;
	} else {
		scale0 = 1.0f - t;
		scale1 = t;
	}

	scale1 = (cosom >= 0.0f) ? scale1 : -scale1;
	return QuadOp::Add(QuadOp::Mul(from, scale0), QuadOp::Mul(to, scale1));
}

inline _VecOp<_Quat, _Quat>::type nextar::Math::_VecOp<_Quat, _Quat>::Inverse(pref q) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_xor_ps(q, N3D_OXXX.v);
#else
	return Set(-q.x, -q.y, -q.z, q.w);
#endif
}

