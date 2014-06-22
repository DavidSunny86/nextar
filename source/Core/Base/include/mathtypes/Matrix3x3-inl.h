// default constructor
//=========================================

inline Matrix3x3::Matrix3x3() {
}

// constructor
//=========================================

inline Matrix3x3::Matrix3x3(const float* _m) {
	MemUtils::CopyDwords(m, _m, 9);
}

// constructor
//=========================================

inline Matrix3x3::Matrix3x3(const float __11, const float __12,
		const float __13, const float __21, const float __22, const float __23,
		const float __31, const float __32, const float __33) {
	_11 = __11;
	_12 = __12;
	_13 = __13;

	_21 = __21;
	_22 = __22;
	_23 = __23;

	_31 = __31;
	_32 = __32;
	_33 = __33;
}

// constructor
//=========================================

inline Matrix3x3::Matrix3x3(const Matrix3x3& _m) {
	MemUtils::CopyDwords(m, _m.m, 9);
}

// constructor
//=========================================

inline Matrix3x3::Matrix3x3(const EularAngles& orientation) {
	*this = orientation;
}

// constructor
//=========================================

inline Matrix3x3::Matrix3x3(const AxisAngle& aa) {
	*this = aa;
}

// constructor
//=========================================

inline Matrix3x3::Matrix3x3(const QuatF q) {
	*this = q;
}

//// operators
//// conversion operators
////=========================================
//inline Matrix3x3::operator const float*() const
//{
//	return m;
//}
//
////=========================================
//inline Matrix3x3::operator float*()
//{
//	return m;
//}

// access grant
//=========================================

inline const Vector3& Matrix3x3::operator ()(int32 i) const {
	NEX_ASSERT(i > 0 && i < 3);
	return *(const Vector3*) (&_11 + i * 3);
}

//=========================================

inline Vector3& Matrix3x3::operator ()(int32 i) {
	NEX_ASSERT(i > 0 && i < 3);
	return *(Vector3*) (&_11 + i * 3);
}

//=========================================

inline const float Matrix3x3::operator ()(int32 i, int32 j) const {
	return m[i * 3 + j];
}

inline float& Matrix3x3::operator ()(int32 i, int32 j) {
	return m[i * 3 + j];
}

//// asignment
////=========================================
//inline Matrix3x3& Matrix3x3::operator = (const float* _m)
//{
//	MemUtils::CopyDwords(m,_m,9);
//	return *this;
//}

//=========================================

inline Matrix3x3& Matrix3x3::operator =(const Matrix3x3& _m) {
	MemUtils::CopyDwords(m, _m.m, 9);
	return *this;
}

//=========================================

inline Matrix3x3& Matrix3x3::operator =(const EularAngles& ea) {
	float sh, ch, sp, cp, sb, cb;
	Math::SinCos(ea.yaw, sh, ch);
	Math::SinCos(ea.pitch, sp, cp);
	Math::SinCos(ea.roll, sb, cb);
	// Fill in the matrix elements
	_11 = ch * cb + sh * sp * sb;
	_12 = -ch * sb + sh * sp * cb;
	_13 = sh * cp;
	_21 = sb * cp;
	_22 = cb * cp;
	_23 = -sp;
	_31 = -sh * cb + ch * sp * sb;
	_32 = sb * sh + ch * sp * cb;
	_33 = ch * cp;
	return *this;
}

//=========================================

inline Matrix3x3& Matrix3x3::operator =(const AxisAngle& aa) {
	Quaternion q = QuatFromAxisAng(aa.axis, aa.angle);
	*this = q;
	return *this;
}

// operations
//=========================================

inline Matrix3x3& Matrix3x3::operator +=(const Matrix3x3& _m) {
	for (int i = 0; i < 9; ++i)
		m[i] += _m.m[i];
	return *this;
}

//=========================================

inline Matrix3x3& Matrix3x3::operator -=(const Matrix3x3& _m) {
	for (int i = 0; i < 9; ++i)
		m[i] -= _m.m[i];
	return *this;
}

//=========================================

inline Matrix3x3& Matrix3x3::operator *=(const Matrix3x3& m2) {
	Matrix3x3 m1 = *this;
	*this = m1 * m2;
	return *this;
} // post multiply

//=========================================

inline Matrix3x3& Matrix3x3::operator *=(const float val) // post multiply
		{
	for (int i = 0; i < 9; ++i)
		m[i] *= val;
	return *this;
}

//=========================================

inline Matrix3x3& Matrix3x3::operator /=(const float val) // post multiply
		{
	*this *= (1 / val);
	return *this;
}

//=========================================
// invert

inline Matrix3x3 Matrix3x3::operator -() const {
	return Matrix3x3(-_11, -_12, -_13, -_21, -_22, -_23, -_31, -_32, -_32);
}

// functions
//=========================================

inline void Matrix3x3::Identity() {
	_11 = 1;
	_12 = 0;
	_13 = 0;
	_21 = 0;
	_22 = 1;
	_23 = 0;
	_31 = 0;
	_32 = 0;
	_33 = 1;
}

//=========================================

inline void Matrix3x3::Transpose() {
	float tmp;
#define TPOSE_SWAP(a,b) tmp = _##a##b; _##a##b = _##b##a; _##b##a = tmp;
	TPOSE_SWAP(1, 2);
	TPOSE_SWAP(1, 3);
	TPOSE_SWAP(2, 3);
#undef TPOSE_SWAP
}

//=========================================
inline void Matrix3x3::Invert() {
	*this = Inverse(*this);
}

//=========================================
inline float Matrix3x3::Determinant() const {
	return (_11 * (_22 * _33 - _23 * _32)) + (_12 * (_23 * _31 - _21 * _33))
			+ (_13 * (_21 * _32 - _31 * _22));
}

//=========================================

inline void Matrix3x3::FromInvEular(const EularAngles& v) {
	EularAngles invm2;
	invm2.yaw = -v.yaw;
	invm2.pitch = -v.pitch;
	invm2.roll = -v.roll;
	*this = invm2;
}

//=========================================
inline void Matrix3x3::FromInvAxisAng(const AxisAngle& aa) {
	Quaternion q = QuatFromAxisAng(aa.axis, aa.angle);
	FromInvQuat(q);
}

//=========================================
inline Vector3 Matrix3x3::Transform(const Vector3& v) const {
	Vector3 rv;
	rv.x = v.x * _11 + v.y * _21 + v.z * _31;
	rv.y = v.x * _12 + v.y * _22 + v.z * _32;
	rv.z = v.x * _13 + v.y * _23 + v.z * _33;
	return rv;
}

//=========================================
inline Vector3 Matrix3x3::InvTransform(const Vector3& v) const {
	Vector3 rv;
	rv.x = Dot(v, *(const Vector3*) &(_11));
	rv.y = Dot(v, *(const Vector3*) &(_21));
	rv.z = Dot(v, *(const Vector3*) &(_31));
	return rv;
}

// friend ops
// operators
//=========================================

inline Vector3 operator *(const Vector3& v, const Matrix3x3& m) {
	return m.Transform(v);
}

//=========================================

inline Vector3 operator *(const Matrix3x3& m, const Vector3& v) {
	return m.InvTransform(v);
}

//=========================================

inline Matrix3x3 operator *(const Matrix3x3& m1, const Matrix3x3& m2) {
	Matrix3x3 m;
	for (int32 i = 0; i < 3; ++i)
		for (int32 j = 0; j < 3; ++j)
			m.m[i * 3 + j] = m1.m[i * 3 + 0] * m2.m[j + 0 * 3]
					+ m1.m[i * 3 + 1] * m2.m[j + 1 * 3]
					+ m1.m[i * 3 + 2] * m2.m[j + 2 * 3];
	return m;
}

//=========================================

inline Matrix3x3 operator +(const Matrix3x3& m1, const Matrix3x3& m2) {
	Matrix3x3 m;
	for (int32 i = 0; i < 9; ++i) {
		m.m[i] = m1.m[i] + m2.m[i];
	}
	return m;
}

//=========================================

inline Matrix3x3 operator -(const Matrix3x3& m1, const Matrix3x3& m2) {
	Matrix3x3 m;
	for (int32 i = 0; i < 9; ++i) {
		m.m[i] = m1.m[i] - m2.m[i];
	}
	return m;
}

//=========================================

inline Matrix3x3 operator *(const Matrix3x3& m1, const float val) {
	Matrix3x3 m;
	for (int32 i = 0; i < 9; ++i) {
		m.m[i] = m1.m[i] * val;
	}
	return m;

}

//=========================================

inline Matrix3x3 operator *(const float v1, const Matrix3x3& m) {
	return m * v1;
}

//=========================================

inline Matrix3x3 operator /(const Matrix3x3& m, const float v1) {
	return m * (1 / v1);
}

//=========================================
inline Matrix3x3 Transpose(const Matrix3x3& m) {
	Matrix3x3 ret(m._11, m._21, m._31, m._12, m._22, m._32, m._13, m._23,
			m._33);

	return ret;
}

