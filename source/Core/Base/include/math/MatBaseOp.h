
#pragoma once

namespace nextar {
namespace Math {

template <typename T>
MatBaseOp<T>::row_type MatBaseOp::Row(pref m, uint32 i) {
	return m.r[i];
}

template <typename T>
MatBaseOp<T>::base_type MatBaseOp::Get(pref m, uint32 i, uint32 j) {
	return VecOp<row_type>::Get(m.r[i], j);
}

template <typename T>
MatBaseOp<T>::type MatBaseOp::Add(pref m1, pref m2) {
	type r;
	for (unsigned int i = 0; i < _rows; ++i)
		r.r[i] = row_op::Add(m1.r[i], m2.r[i]);
	return r;
}

template <typename T>
MatBaseOp<T>::type MatBaseOp::Sub(pref m1, pref m2) {
	type r;
	for (unsigned int i = 0; i < _rows; ++i)
		r.r[i] = row_op::Sub(m1.r[i], m2.r[i]);
	return r;
}

template<typename T>
inline _Mat4Op<T>::type _Mat4Op<T>::FromVectorMapping(TraitsVec3::pref v1, TraitsVec3::pref v2) {
	/** \todo sse **/
	type m;
	float_type cs, xy1C, xz1C, yz1C;
	cs = Vec3Op::Dot(axis1, axis2);
	float_type _1_c;
	Vector3 axis = Vec3Op::Cross(axis1, axis2);
	//OPTIMIZE: we can also check the angle to
	// see if its a multiple of Pi.
	if (Math::Abs(axis.x) < Math::EPSILON_MED
		&& Math::Abs(axis.y) < Math::EPSILON_MED
		&& Math::Abs(axis.z) < Math::EPSILON_MED) {
		// take a cross for that
		axis = Vec3Op::Cross(axis1, Vector3::YAxis);
		if (Math::Abs(axis.x) < Math::EPSILON_MED
			&& Math::Abs(axis.y) < Math::EPSILON_MED
			&& Math::Abs(axis.z) < Math::EPSILON_MED) {
			axis = Vec3Op::Cross(axis1, Vector3::XAxis);
		}
	}
	_1_c = 1.0f - cs;
	Vector3 xyzs = Vec3Op::Mul(axis, -Math::Sqrt(1 - cs * cs));
	Vector3 mstr = Vec3Op::Mul(axis, axis);
	mstr = mstr * _1_c;
	xy1C = axis.x * axis.y * _1_c;
	xz1C = axis.x * axis.z * _1_c;
	yz1C = axis.y * axis.z * _1_c;

	m.r[0] = row_op::Set(cs + mstr.x, xy1C - xyzs.z, xz1C + xyzs.y, 0);
	m.r[1] = row_op::Vec4ASet(xy1C + xyzs.z, cs + mstr.y, yz1C - xyzs.x, 0);
	m.r[2] = row_op::Set(xz1C - xyzs.y, yz1C + xyzs.x, cs + mstr.z, 0);

	return m;
}

template<typename T>
inline void _Mat4Op<T>::Rotate(pref m, TraitsVec3::type * iStream, uint32 inStride, uint32 count) {
	NEX_ASSERT(iStream);
	const uint8* inpVec = (const uint8*)iStream;
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	uint8* outVec = (uint8*)iStream;
	union {
		Quad v;
		float_type s[4];
	}store;
	

	for (uint32 i = 0; i < count; i++) {
		Quad x = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->x);
		Quad y = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->y);
		Quad res = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->z);
		res = _mm_mul_ps(res, m.r[2]);
		y = _mm_mul_ps(y, m.r[1]);
		res = _mm_add_ps(res, y);
		x = _mm_mul_ps(x, m.r[0]);
		res = _mm_add_ps(res, x);
		res = Vec3AOp::Normalize(res);
		_mm_store_ps(store.s, res);
		((float_type*)inpVec)[0] = store.s[0];
		((float_type*)inpVec)[1] = store.s[1];
		((float_type*)inpVec)[2] = store.s[2];

		inpVec += inStride;
	}
#else
	Quad x, y, z, r;
	for (uint32 i = 0; i < count; i++) {
		x = QuadOp::Set(((float_type*)inpVec)[0]);
		y = QuadOp::Set(((float_type*)inpVec)[1]);
		z = QuadOp::Set(((float_type*)inpVec)[2])

		r = Vec3AOp::Mul(z, Row(m,2));
		r = Vec3AOp::Madd(y, Row(m, 1), r);
		r = Vec3AOp::Normalize(Vec3AOp::Madd(x, Row(m, 0), r));

		((float_type*)inpVec)[0] = r.x;
		((float_type*)inpVec)[1] = r.y;
		((float_type*)inpVec)[2] = r.z;

		inpVec += inStride;
	}
#endif
}

template<typename T>
inline TraitsVec3A::type _Mat4Op<T>::Rotate(pref m, TraitsVec3A::pref v) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	Quad vRes = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
	vRes = _mm_mul_ps(vRes, m.r[0]);
	Quad vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
	vTemp = _mm_mul_ps(vTemp, m.r[1]);
	vRes = _mm_add_ps(vRes, vTemp);
	vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
	vTemp = _mm_mul_ps(vTemp, m.r[2]);
	vRes = _mm_add_ps(vRes, vTemp);
	return vRes;
#else
	Quad r = Vec3AOp::Mul(Vec3AOp::SplatZ(v), Row(m, 2));
	r = Vec3AOp::Madd(Vec3AOp::SplatY(v), Row(m, 1), r);
	r = Vec3AOp::Madd(Vec3AOp::SplatX(v), Row(m, 0), r);
	return r;
#endif
}

template<typename T>
inline void _Mat4Op<T>::SetRot(ref ret, TraitsQuat::pref rot) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	Quad r0, r1, r2;
	Quad q0, q1;
	Quad v0, v1, v2;

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
	ret.r[0] = q1;
	q1 = _mm_shuffle_ps(r0, v0, _MM_SHUFFLE(3, 2, 3, 1));
	q1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 3, 0, 2));
	ret.r[1] = q1;
	q1 = _mm_shuffle_ps(v1, r0, _MM_SHUFFLE(3, 2, 1, 0));
	ret.r[2] = q1;
#else
	float_type xx = rot.x * rot.x;
	float_type yy = rot.y * rot.y;
	float_type zz = rot.z * rot.z;
	float_type xy = rot.x * rot.y;
	float_type xz = rot.x * rot.z;
	float_type yz = rot.y * rot.z;
	float_type wx = rot.w * rot.x;
	float_type wy = rot.w * rot.y;
	float_type wz = rot.w * rot.z;

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
#endif
}


template<typename T>
inline _Mat4Op<T>::type _Mat4Op<T>::FromQuat(TraitsQuat::pref rot) {
	type r;
	SetRot(r, rot);
	return r;
}

template<typename T>
inline _Mat4Op<T>::type _Mat4Op<T>::FromRot(TraitsQuat::pref rot) {
	return FromQuat(rot);
}

template<typename T>
inline _Mat4Op<T>::type _Mat4Op<T>::SetViewAndUp(ref ret, TraitsVec3A::pref viewDir, TraitsVec3A::pref up) {
	Vector3A up;
	ret.r[2] = Vec3AOp::Normalize(viewDir);
	up = Vec3AOp::Cross(upDir, viewDir);
	ret.r[1] = Vec3AOp::Normalize(up);
	ret.r[0] = Vec3AOp::Cross(ret.r[2], ret.r[1]);
}

}
}