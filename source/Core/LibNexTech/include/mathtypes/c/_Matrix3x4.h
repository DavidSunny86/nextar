_NexInline Matrix3x4 Mat3x4FromVectorMapping(const Vector3& axis1,
		const Vector3& axis2) {
	/** \@todo sse **/
	Matrix3x4 m;
	float cs, xy1C, xz1C, yz1C;
	Vector3 xyzs, mstr;

	cs = Dot(axis1, axis2);
	{
		Vector3 axis;
		float _1_c;
		axis = Cross(axis1, axis2);
		//OPTIMIZE: we can also check the angle to
		// see if its a multiple of Pi.
		if (fabsf(axis.x) < N3DEpsilonMed && fabsf(axis.y) < N3DEpsilonMed
				&& fabsf(axis.z) < N3DEpsilonMed) {
			// take a cross for that
			axis = Cross(axis1, Vector3::YAxis);
			if (fabsf(axis.x) < N3DEpsilonMed && fabsf(axis.y) < N3DEpsilonMed
					&& fabsf(axis.z) < N3DEpsilonMed) {
				axis = Cross(axis1, Vector3::XAxis);
			}
		}
		_1_c = 1.0f - cs;
		xyzs = axis * -Math::Sqrt((1 - cs * cs));
		mstr = axis * axis;
		mstr = mstr * _1_c;
		xy1C = axis.x * axis.y * _1_c;
		xz1C = axis.x * axis.z * _1_c;
		yz1C = axis.y * axis.z * _1_c;
	}

	m.Row(0) = Quad(cs + mstr.x, xy1C - xyzs.z, xz1C + xyzs.y, 0);

	m.Row(1) = Quad(xy1C + xyzs.z, cs + mstr.y, yz1C - xyzs.x, 0);

	m.Row(2) = Quad(xz1C - xyzs.y, yz1C + xyzs.x, cs + mstr.z, 0);

	return m;
}

_NexInline Matrix3x4 Mat3x4Transpose(Mat3x4F m) {
	// basic swapping
	// 
	//  a   b   c
	//  d   e   f
	//  g   h   i
	// swap(b,d) swap(g,c), swap(h,f)
	Matrix3x4 ret = m;
	std::swap(ret.m01, ret.m10);
	std::swap(ret.m12, ret.m21);
	std::swap(ret.m02, ret.m20);
	return ret;
}

_NexInline Matrix3x4 Mat3x4FromViewUp(Vec3AF viewDir, Vec3AF upDir) {
	Matrix3x4 ret;
	ret.Row(2) = Vec3ANormalize(viewDir);
	ret.Row(0) = Vec3ANormalize(Vec3ACross(upDir, viewDir));
	ret.Row(1) = Vec3ACross(ret.Row(2), ret.Row(0));
	return ret;
}
