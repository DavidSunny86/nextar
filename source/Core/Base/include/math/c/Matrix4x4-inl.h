namespace nextar {


inline void Mat4x4TransVec3(Vector4* outstream, uint32 outstride,
		const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m) {
	NEX_ASSERT(outstream);
	NEX_ASSERT(inpstream);
	const uint8* inpVec = (const uint8*) inpstream;
	uint8* outVec = (uint8*) outstream;

	Quad v, x, y, z, r;

	for (uint32 i = 0; i < count; i++) {

		v.Load3((float*) inpVec);

		z = v.SplatZ();
		y = v.SplatY();
		x = v.SplatX();

		r = Vec3AMulAdd(z, m.Row(2), m.Row(3));
		r = Vec3AMulAdd(y, m.Row(1), r);
		r = Vec3AMulAdd(x, m.Row(0), r);

		r.Store4((float*) outVec);

		inpVec += inpstride;
		outVec += outstride;
	}
}

inline void Mat4x4TransVec3(Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m) {
	NEX_ASSERT(inpstream);
	const uint8* inpVec = (const uint8*)inpstream;
	
	Quad v, x, y, z, r;

	for (uint32 i = 0; i < count; i++) {

		v.Load3((float*)inpVec);
		z = v.SplatZ();
		y = v.SplatY();
		x = v.SplatX();

		r = Vec3AMulAdd(z, m.Row(2), m.Row(3));
		r = Vec3AMulAdd(y, m.Row(1), r);
		r = Vec3AMulAdd(x, m.Row(0), r);

		((float*)inpVec)[0] = r.x;
		((float*)inpVec)[1] = r.y;
		((float*)inpVec)[2] = r.z;


		inpVec += inpstride;
	}
}

inline void Mat4x4TransVec3Normals(Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m) {
	NEX_ASSERT(inpstream);
	const uint8* inpVec = (const uint8*)inpstream;

	Quad v, x, y, z, r;

	for (uint32 i = 0; i < count; i++) {

		v.Load3((float*)inpVec);
		z = v.SplatZ();
		y = v.SplatY();
		x = v.SplatX();

		r = Vec3AMul(z, m.Row(2));
		r = Vec3AMulAdd(y, m.Row(1), r);
		r = Vec3ANormalize(Vec3AMulAdd(x, m.Row(0), r));

		((float*)inpVec)[0] = r.x;
		((float*)inpVec)[1] = r.y;
		((float*)inpVec)[2] = r.z;


		inpVec += inpstride;
	}
}

inline void Mat4x4TransAndProjVec3(Vector3* outstream, uint32 outstride,
		const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m) {
	NEX_ASSERT(outstream);
	NEX_ASSERT(inpstream);

	NEX_ASSERT(outstream);
	NEX_ASSERT(inpstream);
	const uint8* inpVec = (const uint8*) inpstream;
	uint8* outVec = (uint8*) outstream;

	Quad v, x, y, z, r;

	for (uint32 i = 0; i < count; i++) {

		v.Load3((float*) inpVec);

		z = v.SplatZ();
		y = v.SplatY();
		x = v.SplatX();

		r = Vec3AMulAdd(z, m.Row(2), m.Row(3));
		r = Vec3AMulAdd(y, m.Row(1), r);
		r = Vec3AMulAdd(x, m.Row(0), r);
		r = Vec3AMulScalar(r, 1.f / r.w);

		r.Store4((float*) outVec);

		inpVec += inpstride;
		outVec += outstride;
	}
}

inline void Mat4x4TransVec3(Vector4A* outstream, uint32 outstride,
		const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m2) {
	Mat4x4TransVec3((Vector4*) outstream, outstride, inpstream, inpstride,
			count, m2);
}

inline Vector3A Mat4x4TransAndProjVec3A(Vec3AF v, Mat4x4F m) {
	Quad r, x, y, z;

	z = v.SplatZ();
	y = v.SplatY();
	x = v.SplatX();

	r = Vec3AMulAdd(z, m.Row(2), m.Row(3));
	r = Vec3AMulAdd(y, m.Row(1), r);
	r = Vec3AMulAdd(x, m.Row(0), r);
	r = Vec3AMulScalar(r, 1.f / r.w);

	return r;
}

inline Vector4A Mat4x4TransVec3A(Vec3AF v, Mat4x4F m) {
	Quad r, x, y, z;

	z = v.SplatZ();
	y = v.SplatY();
	x = v.SplatX();

	r = Vec3AMulAdd(z, m.Row(2), m.Row(3));
	r = Vec3AMulAdd(y, m.Row(1), r);
	r = Vec3AMulAdd(x, m.Row(0), r);

	return r;
}

inline Vector4A Mat4x4TransBoundRadius(Vec3AF v, Mat4x4F m) {
	Vector3A ret;
	for (int i = 0; i < 3; i++) {
		ret.v[i] = 0.0f;
		for (int j = 0; j < 3; j++) {
			ret.v[i] += v.v[j] * Math::Abs(m.m[i + j * 4]);
		}
	}
	return ret;
}

inline AxisAlignedBox Mat4x4TransAABox(AABoxF box, Mat4x4F m) {
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
}

inline Matrix4x4 Mat4x4FromVectorMapping(const Vector3& axis1,
		const Vector3& axis2) {
	Matrix3x4 m = Mat3x4FromVectorMapping(axis1, axis2);
	return Matrix4x4(m, Matrix4x4::IdentityMatrix.Row(3));
}

inline Matrix4x4 Mat4x4FromVectorMapping(const Vector3& axis1,
		const Vector3& axis2, const Vector3& t) {
	Matrix3x4 m = Mat3x4FromVectorMapping(axis1, axis2);
	return Matrix4x4(m, Vec4ASet(t.x, t.y, t.z, 1));
}

inline Matrix4x4 Mat4x4Mul(Mat4x4F m1, Mat4x4F m2) {
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
}

inline Matrix4x4 Mat4x4FromScaleRotPos(float scale, QuatF rot, Vec3AF pos) {
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
}

inline Matrix4x4 Mat4x4FromScale(Vec3AF scale) {
	Matrix4x4 ret;
	ret.m00 = scale.x; ret.m01 = 0; ret.m02 = 0; ret.m03 = 0;
	ret.m10 = 0; ret.m11 = scale.y; ret.m12 = 0; ret.m13 = 0;
	ret.m20 = 0; ret.m21 = 0; ret.m22 = scale.z; ret.m23 = 0;
	ret.m30 = 0; ret.m31 = 0; ret.m32 = 0; ret.m33 = 1;
	return ret;
}

inline Matrix4x4 Mat4x4FromPos(Vec3AF pos) {
	Matrix4x4 ret;
	ret.m00 = 1; ret.m01 = 0; ret.m02 = 0; ret.m03 = 0;
	ret.m10 = 0; ret.m11 = 1; ret.m12 = 0; ret.m13 = 0;
	ret.m20 = 0; ret.m21 = 0; ret.m22 = 1; ret.m23 = 0;
	ret.m30 = pos.x; ret.m31 = pos.y; ret.m32 = pos.z; ret.m33 = 1;
	return ret;
}

inline Matrix4x4 Mat4x4FromRot(QuatF rot) {
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

	ret.m00 = (1 - 2 * (yy + zz));
	ret.m01 = (2 * (xy + wz));
	ret.m02 = (2 * (xz - wy));
	ret.m03 = 0;

	ret.m10 = (2 * (xy - wz));
	ret.m11 = (1 - 2 * (xx + zz));
	ret.m12 = (2 * (yz + wx));
	ret.m13 = 0;

	ret.m20 = (2 * (xz + wy));
	ret.m21 = (2 * (yz - wx));
	ret.m22 = (1 - 2 * (xx + yy));
	ret.m23 = 0;
		
	ret.m30 = 0;
	ret.m31 = 0;
	ret.m32 = 0;
	ret.m33 = 1.0f;

	return ret;
}

inline Matrix4x4 Mat4x4Scale(float scale, Mat4x4F m) {
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
}

inline Matrix4x4 Mat4x4Transpose(Mat4x4F m) {

	Matrix4x4 ret;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			ret.m[i * 4 + j] = m.m[j * 4 + i];

	return ret;
}

inline Matrix4x4 Mat4x4ViewFromWorld(Mat4x4F m) {
	// View = [ T(R)       0 ]
	//        [ -vpos*T(R) 1 ]
	Matrix4x4 ret;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j)
			ret.m[i * 4 + j] = m.m[j * 4 + i];
		ret.m[i * 4 + 3] = 0;
	}

	ret.m30 = -Vec3ADot(m.Row(3), m.Row(0));
	ret.m31 = -Vec3ADot(m.Row(3), m.Row(1));
	ret.m32 = -Vec3ADot(m.Row(3), m.Row(2));
	ret.m33 = 1;
	return ret;
}

inline Matrix4x4 Mat4x4FromCameraLookAt(Vec3AF eye, Vec3AF lookat, Vec3AF vup) {
	Vector3A zaxis = Vec3ANormalize(Vec3ASub(lookat, eye));
	Vector3A xaxis = Vec3ANormalize(Vec3ACross(vup, zaxis));
	Vector3A yaxis = Vec3ACross(zaxis, xaxis);
	return Matrix4x4(
		xaxis.x, yaxis.x, zaxis.x, 0, 
		xaxis.y, yaxis.y, zaxis.y, 0,
		xaxis.z, yaxis.z, zaxis.z, 0, 
		-Vec3ADot(xaxis, eye), -Vec3ADot(yaxis, eye), -Vec3ADot(zaxis, eye), 1);
}

inline Matrix4x4 Mat4x4FromOrtho(float w, float h, float zn, float zf) {
	float dzRecip = 1.0f / (zf - zn);

	return Matrix4x4(2 / w, 0, 0, 0, 0, 2 / h, 0, 0, 0, 0, -2 * dzRecip, 0, 0,
			0, -(zn + zf) * dzRecip, 1);
}

inline Matrix4x4 Mat4x4FromPerspective(float fieldOfView, float aspectRatio,
		float zn, float zf) {

	fieldOfView *= 0.5f;

	float yscale = 1 / Math::Tan(fieldOfView);
	float q = zf / (zf - zn);
	return Matrix4x4((yscale / aspectRatio), 0, 0, 0, 0, yscale, 0, 0, 0, 0, q,
			1, 0, 0, -q * zn, 0);
}

inline Matrix4x4 Mat4x4Inverse(Mat4x4F m) {
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
}

}  // namespace nextar
