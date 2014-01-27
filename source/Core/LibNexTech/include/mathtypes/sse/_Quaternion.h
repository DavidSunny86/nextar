#include "_Vector3a.h"

#ifndef NEXTAR_MATH_SSETYPES_H
#error Do not include this file directly, Include  NexMath.h
#endif

/** @remarks Quat from Axis Angle (Unoptimized causing Load/Store hits) **/
_NexInline Quaternion QuatFromAxisAng(const Vector3& axis, float ang) {
#if NEX_CORE_DEBUG_CHECKS == 1
	float len = axis.Length();
	NEX_ASSERT(NEX_FLOAT_TOLERANCE_EQUAL(len, 1, N3DEpsilonMed));
#endif
	float f, c;
	Math::SinCos(ang * .5f, f, c);
	return QuadSet(f * axis.x, f * axis.y, f * axis.z, c);
}

_NexInline Quaternion QuatFromAxisAng(Vec3AF axis, float ang) {
#if NEX_CORE_DEBUG_CHECKS == 1
	float len = 1; //\@todo Find length of axis
	NEX_ASSERT(NEX_FLOAT_TOLERANCE_EQUAL(len, 1, N3DEpsilonMed));
#endif
	Quad N = _mm_and_ps(axis, N3D_FFFO.v);
	N = _mm_or_ps(N, N3D_0001.v);
	Quad vSine;
	Quad vCosine;
	QuadSinCos(vSine, vCosine, 0.5f * ang);
	vSine = _mm_and_ps(vSine, N3D_FFFO.v);
	vCosine = _mm_and_ps(vCosine, N3D_OOOF.v);
	return _mm_mul_ps(N, _mm_or_ps(vSine, vCosine));
}

_NexInline Quaternion QuatFromMat4x4(Mat4x4F m) {
	return QuatFromMat3x4(*(const Matrix3x4*)&m);
}

_NexInline Quaternion QuatFromMat3x4(Mat3x4F m) {
	Quad ret = m.r[0];
#if 0
	XMVECTOR Q0, Q1, Q2;
	XMVECTOR M00, M11, M22;
	XMVECTOR CQ0, CQ1, C;
	XMVECTOR CX, CY, CZ, CW;
	XMVECTOR SQ1, Scale;
	XMVECTOR Rsq, Sqrt, VEqualsNaN;
	XMVECTOR A, B, P;
	XMVECTOR PermuteSplat, PermuteSplatT;
	XMVECTOR SignB, SignBT;
	XMVECTOR PermuteControl, PermuteControlT;
	XMVECTOR Result;
	static CONST XMVECTORF32
	OneQuarter = {0.25f, 0.25f, 0.25f, 0.25f};
	static CONST XMVECTORF32
	SignPNNP = {1.0f, -1.0f, -1.0f, 1.0f};
	static CONST XMVECTORF32
	SignNPNP = {-1.0f, 1.0f, -1.0f, 1.0f};
	static CONST XMVECTORF32
	SignNNPP = {-1.0f, -1.0f, 1.0f, 1.0f};
	static CONST XMVECTORF32
	SignPNPP = {1.0f, -1.0f, 1.0f, 1.0f};
	static CONST XMVECTORF32
	SignPPNP = {1.0f, 1.0f, -1.0f, 1.0f};
	static CONST XMVECTORF32
	SignNPPP = {-1.0f, 1.0f, 1.0f, 1.0f};
	static CONST XMVECTORU32
	Permute0X0X0Y0W = {XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W};
	static CONST XMVECTORU32
	Permute0Y0Z0Z1W = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_1W};
	static CONST XMVECTORU32
	SplatX = {XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X};
	static CONST XMVECTORU32
	SplatY = {XM_PERMUTE_0Y, XM_PERMUTE_0Y, XM_PERMUTE_0Y, XM_PERMUTE_0Y};
	static CONST XMVECTORU32
	SplatZ = {XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0Z};
	static CONST XMVECTORU32
	SplatW = {XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0W};
	static CONST XMVECTORU32
	PermuteC = {XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Y};
	static CONST XMVECTORU32
	PermuteA = {XM_PERMUTE_0Y, XM_PERMUTE_1Y, XM_PERMUTE_1Z, XM_PERMUTE_0W};
	static CONST XMVECTORU32
	PermuteB = {XM_PERMUTE_1X, XM_PERMUTE_1W, XM_PERMUTE_0Z, XM_PERMUTE_0W};
	static CONST XMVECTORU32
	Permute0 = {XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1Z, XM_PERMUTE_1Y};
	static CONST XMVECTORU32
	Permute1 = {XM_PERMUTE_1X, XM_PERMUTE_0Y, XM_PERMUTE_1Y, XM_PERMUTE_1Z};
	static CONST XMVECTORU32
	Permute2 = {XM_PERMUTE_1Z, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_1X};
	static CONST XMVECTORU32
	Permute3 = {XM_PERMUTE_1Y, XM_PERMUTE_1Z, XM_PERMUTE_1X, XM_PERMUTE_0W};

	M00 = XMVectorSplatX(M.r[0]);
	M11 = XMVectorSplatY(M.r[1]);
	M22 = XMVectorSplatZ(M.r[2]);

	Q0 = XMVectorMultiply(SignPNNP.v, M00);
	Q0 = XMVectorMultiplyAdd(SignNPNP.v, M11, Q0);
	Q0 = XMVectorMultiplyAdd(SignNNPP.v, M22, Q0);

	Q1 = XMVectorAdd(Q0, g_XMOne.v);

	Rsq = XMVectorReciprocalSqrt(Q1);
	VEqualsNaN = XMVectorIsNaN(Rsq);
	Sqrt = XMVectorMultiply(Q1, Rsq);
	Q1 = XMVectorSelect(Sqrt, Q1, VEqualsNaN);

	Q1 = XMVectorMultiply(Q1, g_XMOneHalf.v);

	SQ1 = XMVectorMultiply(Rsq, g_XMOneHalf.v);

	CQ0 = XMVectorPermute(Q0, Q0, Permute0X0X0Y0W.v);
	CQ1 = XMVectorPermute(Q0, g_XMEpsilon.v, Permute0Y0Z0Z1W.v);
	C = XMVectorGreaterOrEqual(CQ0, CQ1);

	CX = XMVectorSplatX(C);
	CY = XMVectorSplatY(C);
	CZ = XMVectorSplatZ(C);
	CW = XMVectorSplatW(C);

	PermuteSplat = XMVectorSelect(SplatZ.v, SplatY.v, CZ);
	SignB = XMVectorSelect(SignNPPP.v, SignPPNP.v, CZ);
	PermuteControl = XMVectorSelect(Permute2.v, Permute1.v, CZ);

	PermuteSplat = XMVectorSelect(PermuteSplat, SplatZ.v, CX);
	SignB = XMVectorSelect(SignB, SignNPPP.v, CX);
	PermuteControl = XMVectorSelect(PermuteControl, Permute2.v, CX);

	PermuteSplatT = XMVectorSelect(PermuteSplat, SplatX.v, CY);
	SignBT = XMVectorSelect(SignB, SignPNPP.v, CY);
	PermuteControlT = XMVectorSelect(PermuteControl, Permute0.v, CY);

	PermuteSplat = XMVectorSelect(PermuteSplat, PermuteSplatT, CX);
	SignB = XMVectorSelect(SignB, SignBT, CX);
	PermuteControl = XMVectorSelect(PermuteControl, PermuteControlT, CX);

	PermuteSplat = XMVectorSelect(PermuteSplat, SplatW.v, CW);
	SignB = XMVectorSelect(SignB, g_XMNegativeOne.v, CW);
	PermuteControl = XMVectorSelect(PermuteControl, Permute3.v, CW);

	Scale = XMVectorPermute(SQ1, SQ1, PermuteSplat);

	P = XMVectorPermute(M.r[1], M.r[2], PermuteC.v); // {M10, M12, M20, M21}
	A = XMVectorPermute(M.r[0], P, PermuteA.v); // {M01, M12, M20, M03}
	B = XMVectorPermute(M.r[0], P, PermuteB.v); // {M10, M21, M02, M03}

	Q2 = XMVectorMultiplyAdd(SignB, B, A);
	Q2 = XMVectorMultiply(Q2, Scale);

	Result = XMVectorPermute(Q1, Q2, PermuteControl);

	return Result;
#endif
	return ret;
}

_NexInline Quaternion QuatMul(QuatF q1, QuatF q2) {

	// Copy to SSE registers and use as few as possible for x86
	Quad Q2X = q2;
	Quad Q2Y = q2;
	Quad Q2Z = q2;
	Quad vResult = q2;
	// Splat with one instruction
	vResult = _mm_shuffle_ps(vResult, vResult, _MM_SHUFFLE(3, 3, 3, 3));
	Q2X = _mm_shuffle_ps(Q2X, Q2X, _MM_SHUFFLE(0, 0, 0, 0));
	Q2Y = _mm_shuffle_ps(Q2Y, Q2Y, _MM_SHUFFLE(1, 1, 1, 1));
	Q2Z = _mm_shuffle_ps(Q2Z, Q2Z, _MM_SHUFFLE(2, 2, 2, 2));
	// Retire q1 and perform q1*Q2W
	vResult = _mm_mul_ps(vResult, q1);
	Quad Q1Shuffle = q1;
	// Shuffle the copies of q1
	Q1Shuffle = _mm_shuffle_ps(Q1Shuffle, Q1Shuffle, _MM_SHUFFLE(0, 1, 2, 3));
	// Mul by Q1WZYX
	Q2X = _mm_mul_ps(Q2X, Q1Shuffle);
	Q1Shuffle = _mm_shuffle_ps(Q1Shuffle, Q1Shuffle, _MM_SHUFFLE(2, 3, 0, 1));
	// Flip the signs on y and z
	Q2X = _mm_xor_ps(Q2X, (N3D_OXOX.v));
	// Mul by Q1ZWXY
	Q2Y = _mm_mul_ps(Q2Y, Q1Shuffle);
	Q1Shuffle = _mm_shuffle_ps(Q1Shuffle, Q1Shuffle, _MM_SHUFFLE(0, 1, 2, 3));
	// Flip the signs on z and w
	Q2Y = _mm_mul_ps(Q2Y, (N3D_OOXX.v));
	// Mul by Q1YXWZ
	Q2Z = _mm_mul_ps(Q2Z, Q1Shuffle);
	vResult = _mm_add_ps(vResult, Q2X);
	// Flip the signs on x and w
	Q2Z = _mm_mul_ps(Q2Z, (N3D_XOOX.v));
	Q2Y = _mm_add_ps(Q2Y, Q2Z);
	vResult = _mm_add_ps(vResult, Q2Y);
	return vResult;
}

_NexInline Vector3A QuatTransVec3A(QuatF q, Vec3AF v) {
	Vector3A uv, uuv;
	uv = Vec3ACross(q, v);
	uuv = Vec3ACross(q, uv);
	return Vec3AAdd(Vec3AAdd(v, Vec3AMulScalar(uv, 2 * QuatGetW(q))),
			Vec3AAdd(uuv, uuv));
}

_NexInline Vector4A QuatTransBoundRadius(QuatF q, Vec3AF v) {
	Vector3A uv, uuv;
	uv = Vec3ACross(q, v);
	uuv = Vec3ACross(q, uv);
	return Vec3AAdd(
			Vec3AAbs(Vec3AAdd(v, Vec3AAbs(Vec3AMulScalar(uv, 2 * QuatGetW(q))))),
					Vec3AAbs(Vec3AAdd(uuv, uuv)));
}

_NexInline Quaternion QuatIdentity() {
	return QuadSet(0, 0, 0, 1);
}

_NexInline Quaternion QuatInverse(QuatF q) {
	// invert the axis
	return _mm_xor_ps(q, N3D_OXXX.v);
}

_NexInline Quaternion QuatSlerp(QuatF from, QuatF to, float t) {

	float cosom, absCosom, sinom, omega, scale0, scale1;
	cosom = Vec4ADot(from, to);
	absCosom = Math::Abs(cosom);
	if ((1.0f - absCosom) > N3DEpsilon) {
		omega = Math::ArcCos(absCosom);
		sinom = 1.0f / Math::Sin(omega);
		scale0 = Math::Sin((1.0f - t) * omega) * sinom;
		scale1 = Math::Sin(t * omega) * sinom;
	} else {
		scale0 = 1.0f - t;
		scale1 = t;
	}

	scale1 = (cosom >= 0.0f) ? scale1 : -scale1;
	return QuadAdd(QuadMulScalar(from, scale0), QuadMulScalar(to, scale1));
}
