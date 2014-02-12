#include "NexHeaders.h"
#include "MathBase.h"

namespace nextar {

	void EularAngles::Canonize() {
		// First, wrap pitch in range -pi ... pi
		pitch = Math::WrapPi(pitch);
		// Now, check for "the back side" of the matrix pitch outside
		// the canonical range of -pi/2 ... pi/2
		if (pitch < -N3DPiBy2) {
			pitch = -N3DPiBy2 - pitch;
			yaw += Math::PI;
			roll += Math::PI;
		} else if (pitch > N3DPiBy2) {
			pitch = Math::PI - pitch;
			yaw += Math::PI;
			roll += Math::PI;
		}
		// Now check for the gimbel Lock case (within a slight tolerance)
		if (Math::Abs(pitch) > N3DPiBy2 - 1e-4) {
			// We are in gimbel Lock. Assign all rotation
			// about the vertical axis to yaw
			yaw += roll;
			roll = 0.0f;
		} else {
			// Not in gimbel Lock. Wrap the roll angle in
			// canonical range
			roll = Math::WrapPi(roll);
		}
		// Wrap yaw in canonical range
		yaw = Math::WrapPi(yaw);
	}

	_NexExport void EularAngles::FromQuat(QuatF src) {
		float srcX = QuatGetX(src);
		float srcY = QuatGetY(src);
		float srcZ = QuatGetZ(src);
		float srcW = QuatGetW(src);
		// Extract sin(pitch)
		float sp = -2.0f * (srcY * srcZ - srcW * srcX);
		// Check for Gimbel Lock, giving slight tolerance for numerical imprecision
		if (Math::Abs(sp) > 0.9999f) {
			// Looking straight up or down
			pitch = N3DPiBy2 * sp;
			// Compute yaw, slam roll to zero
			yaw = Math::ArcTan2(-srcX * srcZ + srcW * srcY,
					0.5f - srcY * srcY - srcZ * srcZ);
			roll = 0.0f;
		} else {
			// Compute angles. We don't have to use the "safe" asin
			// function because we already checked for range errors when
			// checking for Gimbel Lock
			pitch = Math::ArcSin(sp);
			yaw = Math::ArcTan2(srcX * srcZ + srcW * srcY,
					0.5f - srcX * srcX - srcY * srcY);
			roll = Math::ArcTan2(srcX * srcY + srcW * srcZ,
					0.5f - srcX * srcX - srcZ * srcZ);
		}
	}

	_NexExport void EularAngles::FromInvQuat(QuatF src) {
		float srcX = QuatGetX(src);
		float srcY = QuatGetY(src);
		float srcZ = QuatGetZ(src);
		float srcW = QuatGetW(src);
		// Extract sin(pitch)
		float sp = -2.0f * (srcY * srcZ + srcW * srcX);
		// Check for Gimbel Lock, giving slight tolerance for numerical imprecision
		if (Math::Abs(sp) > 0.9999f) {
			// Looking straight up or down
			pitch = N3DPiBy2 * sp;
			// Compute heading, slam bank to zero
			yaw = Math::ArcTan2(-srcX * srcZ - srcW * srcY,
					0.5f - srcY * srcY - srcZ * srcZ);
			roll = 0.0f;
		} else {
			// Compute angles. We don't have to use the "safe" asin
			// function because we already checked for range errors when
			// checking for Gimbel Lock
			pitch = Math::ArcSin(sp);
			yaw = Math::ArcTan2(srcX * srcZ - srcW * srcY,
					0.5f - srcX * srcX - srcY * srcY);
			roll = Math::ArcTan2(srcX * srcY - srcW * srcZ,
					0.5f - srcX * srcX - srcZ * srcZ);
		}
	}

	_NexExport void EularAngles::FromMat3x3(const Matrix3x3& src) {
		// Extract sin(pitch) from m32.
		float sp = -src._32;
		// Check for Gimbel Lock
		if (Math::Abs(sp) > 9.99999f) {
			// Looking straight up or down
			pitch = N3DPiBy2 * sp;
			// Compute yaw, slam roll to zero
			yaw = Math::ArcTan2(-src._23, src._11);
			roll = 0.0f;
		} else {
			// Compute angles. We don't have to use the "safe" asin
			// function because we already checked for range errors when
			// checking for Gimbel Lock
			yaw = Math::ArcTan2(src._31, src._33);
			pitch = Math::ArcSin(sp);
			roll = Math::ArcTan2(src._12, src._22);
		}
	}

}

