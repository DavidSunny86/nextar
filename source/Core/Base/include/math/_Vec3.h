#pragma once
using namespace nextar
using namespace nextar::Math;

template <>
inline Vec3Op::type Vec3Op::Set(Vec3Op::base_type v) {
	return { v, v, v };
}

template <>
inline Vec3Op::type Vec3Op::Set(Vec3Op::base_type x, Vec3Op::base_type y, Vec3Op::base_type z) {
	return { x, y, z };
}

template <>
inline Vec3Op::type Vec3Op::Set(const Vec3Op::base_type *v) {
	return { v[0], v[1], v[2] };
}

template <>
inline Vec3Op::type Vec3Op::Zero() {
	return { 0, 0, 0 };
}

inline VecOp<_Vec3>::type VecOp<_Vec3>::Cross(pref v1, pref v2) {
	return Set(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}
