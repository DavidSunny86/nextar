#pragma once

namespace nextar {

inline Matrix3x4 Mat3x4FromVectorMapping(const Vector3& axis1,
		const Vector3& axis2) {
	/** \todo sse **/
	Matrix3x4 m;
	float cs, xy1C, xz1C, yz1C;

	cs = Dot(axis1, axis2);

	float _1_c;
	Vector3 axis = Cross(axis1, axis2);
	//OPTIMIZE: we can also check the angle to
	// see if its a multiple of Pi.
	if (Math::Abs(axis.x) < Math::EPSILON_MED
			&& Math::Abs(axis.y) < Math::EPSILON_MED
			&& Math::Abs(axis.z) < Math::EPSILON_MED) {
		// take a cross for that
		axis = Cross(axis1, Vector3::YAxis);
		if (Math::Abs(axis.x) < Math::EPSILON_MED
				&& Math::Abs(axis.y) < Math::EPSILON_MED
				&& Math::Abs(axis.z) < Math::EPSILON_MED) {
			axis = Cross(axis1, Vector3::XAxis);
		}
	}
	_1_c = 1.0f - cs;
	Vector3 xyzs = axis * -Math::Sqrt(1 - cs * cs);
	Vector3 mstr = axis * axis;
	mstr = mstr * _1_c;
	xy1C = axis.x * axis.y * _1_c;
	xz1C = axis.x * axis.z * _1_c;
	yz1C = axis.y * axis.z * _1_c;

	m.r[0] = Vec4ASet(cs + mstr.x, xy1C - xyzs.z, xz1C + xyzs.y, 0);

	m.r[1] = Vec4ASet(xy1C + xyzs.z, cs + mstr.y, yz1C - xyzs.x, 0);

	m.r[2] = Vec4ASet(xz1C - xyzs.y, yz1C + xyzs.x, cs + mstr.z, 0);

	return m;
}

inline Matrix3x4 Mat3x4Transpose(Mat3x4F m) {
	// basic swapping
	//
	//  a   b   c
	//  d   e   f
	//  g   h   i
	// swap(b,d) swap(g,c), swap(h,f)
	Matrix3x4 ret;
	//    std::swap(m.m01, m.m10);
	//    std::swap(m.m02, m.m20);
	//    std::swap(m.m12, m.m21);
	ret.r[0] = _mm_move_ss(
			_mm_shuffle_ps(m.r[1], m.r[2], _MM_SHUFFLE(3, 0, 0, 3)), m.r[0]);
	ret.r[1] = _mm_shuffle_ps(
			_mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(3, 1, 3, 1)), m.r[2],
			_MM_SHUFFLE(3, 1, 2, 0));
	ret.r[2] = _mm_shuffle_ps(
			_mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(3, 2, 3, 2)), m.r[2],
			_MM_SHUFFLE(3, 2, 2, 0));
	return ret;
}

inline Vec3AF Mat3x4TransVec3A(Vec3AF v, Mat3x4F m) {
	Quad vRes = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
	vRes = _mm_mul_ps(vRes, m.r[0]);
	Quad vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
	vTemp = _mm_mul_ps(vTemp, m.r[1]);
	vRes = _mm_add_ps(vRes, vTemp);
	vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
	vTemp = _mm_mul_ps(vTemp, m.r[2]);
	vRes = _mm_add_ps(vRes, vTemp);
	return vRes;
}

inline Matrix3x4 Mat3x4FromViewUp(Vec3AF viewDir, Vec3AF upDir) {
	Matrix3x4 ret;
	Vector3A up;
	ret.r[2] = Vec3ANormalize(viewDir);
	up = Vec3ACross(upDir, viewDir);
	ret.r[1] = Vec3ANormalize(up);
	ret.r[0] = Vec3ACross(ret.r[2], ret.r[1]);
	return ret;
}

}
