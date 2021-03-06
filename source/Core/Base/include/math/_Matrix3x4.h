#pragma once
namespace nextar {
namespace Math {


inline MatOp<_Matrix3x4>::type MatOp<_Matrix3x4>::Transpose(pref m) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
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

#else
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

#endif
}

inline MatOp<_Matrix3x4>::type MatOp<_Matrix3x4>::FromQuat(TraitsQuat::pref rot) {
	type ret;
	SetRot(ret, rot);
	return ret;
}

inline MatOp<_Matrix3x4>::type MatOp<_Matrix3x4>::FromRot(TraitsQuat::pref rot) {
	return FromQuat(rot);
}

}
}
