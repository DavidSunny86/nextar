#pragma once
namespace nextar {
namespace Math {

inline typename _VecOp<_Vec3>::type _VecOp<_Vec3>::Cross(pref v1, pref v2) {
	return Set(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}

}
}