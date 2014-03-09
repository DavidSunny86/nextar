#include <BaseHeaders.h>

namespace nextar {

	const Matrix3x3 Matrix3x3::IdentityMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);

	Matrix3x3& Matrix3x3::operator =(const QuatF q) {
		float qX = QuatGetX(q);
		float qY = QuatGetY(q);
		float qZ = QuatGetZ(q);
		float qW = QuatGetW(q);

		float xx, xy, xz, xw, yy, yz, yw, zz, zw;
		xx = qX * qX;
		xy = qX * qY;
		xz = qX * qZ;
		xw = qX * qW;
		yy = qY * qY;
		yz = qY * qZ;
		yw = qY * qW;
		zz = qZ * qZ;
		zw = qZ * qW;

		_11 = 1 - 2 * (yy + zz);
		_12 = 2 * (xy + zw);
		_13 = 2 * (xz - yw);
		_21 = 2 * (xy - zw);
		_22 = 1 - 2 * (xx + zz);
		_23 = 2 * (yz + xw);
		_31 = 2 * (xz + yw);
		_32 = 2 * (yz - xw);
		_33 = 1 - 2 * (xx + yy);
		return *this;
	}

	void Matrix3x3::FromInvQuat(const QuatF q) {
		float qX = QuatGetX(q);
		float qY = QuatGetY(q);
		float qZ = QuatGetZ(q);
		float qW = QuatGetW(q);
		float xx, xy, xz, xw, yy, yz, yw, zz, zw;

		xx = qX * qX;
		xy = qX * qY;
		xz = qX * qZ;
		xw = qX * qW;
		yy = qY * qY;
		yz = qY * qZ;
		yw = qY * qW;
		zz = qZ * qZ;
		zw = qZ * qW;
		_11 = 1 - 2 * (yy + zz);
		_12 = 2 * (xy - zw);
		_13 = 2 * (xz + yw);
		_21 = 2 * (xy + zw);
		_22 = 1 - 2 * (xx + zz);
		_23 = 2 * (yz - xw);
		_31 = 2 * (xz - yw);
		_32 = 2 * (yz + xw);
		_33 = 1 - 2 * (xx + yy);
	}

	_NexBaseExport Matrix3x3 Inverse(const Matrix3x3& m2) {
		Matrix3x3 m1;
		float d;
		m1._11 = (m2._22 * m2._33 - m2._23 * m2._32);
		m1._21 = (m2._23 * m2._31 - m2._21 * m2._33);
		m1._31 = (m2._21 * m2._32 - m2._31 * m2._22);
		d = (m2._11 * m1._11 + m2._12 * m1._21 + m2._13 * m1._31);
		if (Math::Abs(d) > Math::EPSILON) {
			d = 1 / d;
			m1._11 *= d;
			m1._12 = d * (m2._13 * m2._32 - m2._12 * m2._33);
			m1._13 = d * (m2._12 * m2._23 - m2._13 * m2._22);
			m1._21 *= d;
			m1._22 = d * (m2._33 * m2._11 - m2._13 * m2._31);
			m1._23 = d * (m2._13 * m2._21 - m2._23 * m2._11);
			m1._31 *= d;
			m1._32 = d * (m2._31 * m2._12 - m2._11 * m2._32);
			m1._33 = d * (m2._11 * m2._22 - m2._21 * m2._12);
		}
		return m1;
	}

}

