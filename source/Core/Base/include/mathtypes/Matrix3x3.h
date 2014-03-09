#ifndef NEXTAR_MATRIX3X3_H
#define NEXTAR_MATRIX3X3_H

//# generic rotation class
//# Matrix3x3 is not aligned and operations in Matrix3x3
//# mostly avoid SSE or other optimization.
//# A row vector rotation matrix in row major format.

class _NexBaseExport Matrix3x3 : public nextar::AllocGeneral {
public:
	static const Matrix3x3 IdentityMatrix;

	//TODO: Non-ISO (Rectify!)

	union {

		struct {
			float _11, _12, _13,
			_21, _22, _23,
			_31, _32, _33;
		};

		struct {
			float m00, m01, m02,
			m10, m11, m12,
			m20, m21, m22;
		};
		float m[9];
	};

	inline Matrix3x3();
	inline Matrix3x3(const float*);
	inline Matrix3x3(
			const float, const float, const float,
			const float, const float, const float,
			const float, const float, const float);
	inline Matrix3x3(const Matrix3x3&);
	inline Matrix3x3(const EularAngles&);
	inline Matrix3x3(const AxisAngle&);
	inline Matrix3x3(QuatF);
	// operators
	// conversion operators
	//inline operator const float*() const;
	//inline operator float*();
	// access grant
	inline const Vector3 & operator ()(int32 i) const;
	inline Vector3 & operator ()(int32 i);
	inline const float operator ()(int32 i, int32 j) const;
	inline float & operator ()(int32 i, int32 j);
	// asignment
	//inline Matrix3x3& operator = (const float*);
	inline Matrix3x3 & operator =(const Matrix3x3&);
	inline Matrix3x3 & operator =(const EularAngles&);
	inline Matrix3x3 & operator =(const AxisAngle&);
	Matrix3x3 & operator =(QuatF);
	// operations
	inline Matrix3x3 & operator +=(const Matrix3x3&);
	inline Matrix3x3 & operator -=(const Matrix3x3&);
	inline Matrix3x3 & operator *=(const Matrix3x3&);// post multiply
	inline Matrix3x3 & operator *=(const float);// post multiply
	inline Matrix3x3 & operator /=(const float);// post multiply
	// negate
	inline Matrix3x3 operator -() const;
	// functions
	inline void Identity();
	inline void Transpose();
	inline void Invert();
	inline float Determinant() const;
	inline void FromInvEular(const EularAngles & v);
	void FromInvQuat(QuatF v);
	inline void FromInvAxisAng(const AxisAngle & v);
	inline Vector3 Transform(const Vector3 & v) const;
	inline Vector3 InvTransform(const Vector3 & v) const;
	// friend ops
	// operators
	friend inline Vector3 operator *(const Vector3& v1, const Matrix3x3 & m);
	friend inline Vector3 operator *(const Matrix3x3& m, const Vector3 & v1);
	friend inline Matrix3x3 operator *(const Matrix3x3& m, const float v1);
	friend inline Matrix3x3 operator *(const float v1, const Matrix3x3 & m);
	friend inline Matrix3x3 operator /(const Matrix3x3& m, const float v1);
	friend inline Matrix3x3 operator *(const Matrix3x3& m1, const Matrix3x3 & m2);
	friend inline Matrix3x3 operator +(const Matrix3x3& m1, const Matrix3x3 & m2);
	friend inline Matrix3x3 operator -(const Matrix3x3& m1, const Matrix3x3 & m2);
	// functions
	friend _NexBaseExport Matrix3x3 Inverse(const Matrix3x3&);
	friend inline Matrix3x3 Transpose(const Matrix3x3&);

};

#endif //NEXTAR_MATRIX3X3_H
