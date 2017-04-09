#include "VectorMethods.h"
#pragma once
using namespace nextar;
using namespace nextar::Math;

inline _VecOp<_EulerAngles, _EulerAngles>::type _VecOp<_EulerAngles, _EulerAngles>::Canonize(pref m) {
	// First, wrap r.x in range -pi ... pi
	type r;
	r.x = Math::WrapPi(m.x);
	r.y = m.y;
	r.z = m.z;
	// Now, check for "the back side" of the matrix r.x outside
	// the canonical range of -pi/2 ... pi/2
	if (r.x < -Math::PI_BY_2) {
		r.x = -Math::PI_BY_2 - r.x;
		r.y += Math::PI;
		r.z += Math::PI;
	} else if (r.x > Math::PI_BY_2) {
		r.x = Math::PI - r.x;
		r.y += Math::PI;
		r.z += Math::PI;
	}
	// Now check for the gimbel Lock case (within a slight tolerance)
	if (Math::Abs(r.x) > Math::PI_BY_2 - 1e-4) {
		// We are in gimbel Lock. Assign all rotation
		// about the vertical axis to r.y
		r.y += r.z;
		r.z = 0.0f;
	} else {
		// Not in gimbel Lock. Wrap the r.z angle in
		// canonical range
		r.z = Math::WrapPi(r.z);
	}
	// Wrap r.y in canonical range
	r.y = Math::WrapPi(r.y);
	return r;
}

inline _VecOp<_EulerAngles, _EulerAngles>::type _VecOp<_EulerAngles, _EulerAngles>::FromQuat(TraitsQuat::pref src) {
	type r;
	float srcX = QuatOp::GetX(src);
	float srcY = QuatOp::GetY(src);
	float srcZ = QuatOp::GetZ(src);
	float srcW = QuatOp::GetW(src);
	// Extract sin(r.x)
	float sp = -2.0f * (srcY * srcZ - srcW * srcX);
	// Check for Gimbel Lock, giving slight tolerance for numerical imprecision
	if (Math::Abs(sp) > 0.9999f) {
		// Looking straight up or down
		r.x = Math::PI_BY_2 * sp;
		// Compute r.y, slam r.z to zero
		r.y = Math::ArcTan2(-srcX * srcZ + srcW * srcY,
			0.5f - srcY * srcY - srcZ * srcZ);
		r.z = 0.0f;
	} else {
		// Compute angles. We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for Gimbel Lock
		r.x = Math::ArcSin(sp);
		r.y = Math::ArcTan2(srcX * srcZ + srcW * srcY,
			0.5f - srcX * srcX - srcY * srcY);
		r.z = Math::ArcTan2(srcX * srcY + srcW * srcZ,
			0.5f - srcX * srcX - srcZ * srcZ);
	}
	return r;
}

inline _VecOp<_EulerAngles, _EulerAngles>::type _VecOp<_EulerAngles, _EulerAngles>::FromInvQuat(TraitsQuat::pref src) {
	type r;
	float srcX = QuatGetX(src);
	float srcY = QuatGetY(src);
	float srcZ = QuatGetZ(src);
	float srcW = QuatGetW(src);
	// Extract sin(r.x)
	float sp = -2.0f * (srcY * srcZ + srcW * srcX);
	// Check for Gimbel Lock, giving slight tolerance for numerical imprecision
	if (Math::Abs(sp) > 0.9999f) {
		// Looking straight up or down
		r.x = Math::PI_BY_2 * sp;
		// Compute heading, slam bank to zero
		r.y = Math::ArcTan2(-srcX * srcZ - srcW * srcY,
			0.5f - srcY * srcY - srcZ * srcZ);
		r.z = 0.0f;
	} else {
		// Compute angles. We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for Gimbel Lock
		r.x = Math::ArcSin(sp);
		r.y = Math::ArcTan2(srcX * srcZ - srcW * srcY,
			0.5f - srcX * srcX - srcY * srcY);
		r.z = Math::ArcTan2(srcX * srcY - srcW * srcZ,
			0.5f - srcX * srcX - srcZ * srcZ);
	}
	return r;
}

inline _VecOp<_EulerAngles, _EulerAngles>::type _VecOp<_EulerAngles, _EulerAngles>::FromMat4x4(TraitsMat4x4::pref m) {
	return FromMat3x4(*reinterpret_cast<const TraitsMat3x4::type*>(&m));
}

inline _VecOp<_EulerAngles, _EulerAngles>::type _VecOp<_EulerAngles, _EulerAngles>::FromMat3x4(TraitsMat3x4::pref src) {
	type r;
	// Extract sin(r.x) from m32.
	float sp = -src.m21;
	// Check for Gimbel Lock
	if (Math::Abs(sp) > 9.99999f) {
		// Looking straight up or down
		r.x = Math::PI_BY_2 * sp;
		// Compute r.y, slam r.z to zero
		r.y = Math::ArcTan2(-src.m12, src.m00);
		r.z = 0.0f;
	} else {
		// Compute angles. We don't have to use the "safe" asin
		// function because we already checked for range errors when
		// checking for Gimbel Lock
		r.y = Math::ArcTan2(src.m20, src.m22);
		r.x = Math::ArcSin(sp);
		r.z = Math::ArcTan2(src.m01, src.m11);
	}
	return r;
}

