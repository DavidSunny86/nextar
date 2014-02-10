namespace nextar {

	inline Quaternion QuatSet(float x, float y, float z, float w) {
		return Quad(x, y, z, w);
	}

	/** @remarks Quat from Axis Angle (Unoptimized causing Load/Store hits) **/
	inline Quaternion QuatFromAxisAng(const Vector3& axis, float ang) {

#if NEX_CORE_DEBUG_CHECKS == 1
		float len = axis.Length();
		NEX_ASSERT(NEX_FLOAT_TOLERANCE_EQUAL(len, 1, Math::EPSILON_MED));
#endif
		float f, c;
		Math::SinCos(ang * .5f, f, c);
		return Quad(f * axis.x, f * axis.y, f * axis.z, c);
	}

	inline Quaternion QuatFromAxisAng(Vec3AF axis, float ang) {
#if NEX_CORE_DEBUG_CHECKS == 1
		float len = Math::Sqrt(Vec3ADot(axis, axis)); //\@todo Find length of axis
		NEX_ASSERT(NEX_FLOAT_TOLERANCE_EQUAL(len, 1, Math::EPSILON_MED));
#endif
		float s, c;
		Math::SinCos(ang * .5f, s, c);
		return Quad(s * axis.x, s * axis.y, s * axis.z, c);
	}

	inline Quaternion QuatFromMat4x4(Mat4x4F m) {
		return QuatFromMat3x4(*(const Matrix3x4*) &m);
	}

	inline Quaternion QuatFromMat3x4(Mat3x4F m) {
		int maxi;
		float maxdiag, trace;

		trace = m.m00 + m.m11 + m.m22 + 1.0f;
		if (trace > 0.0f) {
			return Quaternion((m.m12 - m.m21) / (2.0f * sqrt(trace)),
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
			return Quaternion(0.25f * s, (m.m01 + m.m10) * invS,
					(m.m02 + m.m20) * invS, (m.m12 - m.m21) * invS);

		case 1:
			s = 2.0f * sqrt(1.0f + m.m11 - m.m00 - m.m22);
			invS = 1 / s;
			return Quaternion((m.m01 + m.m10) * invS, 0.25f * s,
					(m.m12 + m.m21) * invS, (m.m20 - m.m02) * invS);
		case 2:
		default:
			s = 2.0f * sqrt(1.0f + m.m22 - m.m00 - m.m11);
			invS = 1 / s;
			return Quaternion((m.m02 + m.m20) * invS, (m.m12 + m.m21) * invS,
					0.25f * s, (m.m01 - m.m10) * invS);
		}
	}

	inline Quaternion QuatMul(QuatF q1, QuatF q2) {
		return Quad(q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
				q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
				q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
				q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
	}

	inline Vector3A QuatTransVec3A(QuatF q, Vec3AF v) {
		Vector3A uv, uuv;
		uv = Vec3ACross(q, v);
		uuv = Vec3ACross(q, uv);
		return Vec3AAdd(Vec3AAdd(v, Vec3AMulScalar(uv, 2 * QuatGetW(q))),
				Vec3AAdd(uuv, uuv));
	}

	inline Vector4A QuatTransBoundRadius(QuatF q, Vec3AF v) {
		Vector3A uv, uuv;
		uv = Vec3ACross(q, v);
		uuv = Vec3ACross(q, uv);
		return Vec3AAdd(
				Vec3AAdd(v, Vec3AAbs(Vec3AMulScalar(uv, 2 * QuatGetW(q)))),
				Vec3AAbs(Vec3AAdd(uuv, uuv)));
	}

	inline Quaternion QuatIdentity() {
		return Quad(0, 0, 0, 1);
	}

	inline Quaternion QuatSlerp(QuatF from, QuatF to, float t) {

		float cosom, absCosom, sinom, omega, scale0, scale1;
		cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;
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
		Quaternion result;
		result.x = scale0 * from.x + scale1 * to.x;
		result.y = scale0 * from.y + scale1 * to.y;
		result.z = scale0 * from.z + scale1 * to.z;
		result.w = scale0 * from.w + scale1 * to.w;
		return result;
	}

	inline Quaternion QuatLerp(QuatF from, QuatF to, float t) {

		float cosom, scale0, scale1, s;
		Quaternion result;

		cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;
		scale0 = 1.0f - t;
		scale1 = (cosom >= 0.0f) ? t : -t;
		result.x = scale0 * from.x + scale1 * to.x;
		result.y = scale0 * from.y + scale1 * to.y;
		result.z = scale0 * from.z + scale1 * to.z;
		result.w = scale0 * from.w + scale1 * to.w;
		s = 1.0f
				/ sqrt(
						result.x * result.x + result.y * result.y
								+ result.z * result.z + result.w * result.w);
		result.x *= s;
		result.y *= s;
		result.z *= s;
		result.w *= s;
	}

	inline Quaternion QuatInverse(QuatF q) {
		// invert the axis
		return Quad(-q.x, -q.y, -q.z, q.w);
	}

}  // namespace nextar
