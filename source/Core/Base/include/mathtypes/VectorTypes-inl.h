#pragma once

#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE
#	include "mathtypes/sse/SSE_VectorTypes-inl.h"
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_FALLBACK
#	include "mathtypes/c/C_VectorTypes-inl.h"
#else
#endif

namespace nextar {

inline _Matrix3x4& _Matrix3x4::operator =(const Matrix3x4& m) {
	r[0] = Mat3x4Row(m, 0);
	r[1] = Mat3x4Row(m, 1);
	r[2] = Mat3x4Row(m, 2);
	return *this;
}

inline _Matrix4x4& _Matrix4x4::operator =(const Matrix4x4& m) {
	r[0] = Mat4x4Row(m, 0);
	r[1] = Mat4x4Row(m, 1);
	r[2] = Mat4x4Row(m, 2);
	r[3] = Mat4x4Row(m, 3);
	return *this;
}

_Matrix4x4::_Matrix4x4(const Matrix3x4& m, const Vector4A& v) {
	r[0] = Mat3x4Row(m, 0);
	r[1] = Mat3x4Row(m, 1);
	r[2] = Mat3x4Row(m, 2);
	r[3] = v;
}

_Matrix4x4::_Matrix4x4(float m00, float m01, float m02, float m03, float m10,
		float m11, float m12, float m13, float m20, float m21, float m22,
		float m23, float m30, float m31, float m32, float m33) {
	r[0] = Vec4ASet(m00, m01, m02, m03);
	r[1] = Vec4ASet(m10, m11, m12, m13);
	r[2] = Vec4ASet(m20, m21, m22, m23);
	r[3] = Vec4ASet(m30, m31, m32, m33);
}
}
