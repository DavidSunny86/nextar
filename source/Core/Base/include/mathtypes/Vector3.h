#ifndef NEXTAR_VECTOR3_H
#define NEXTAR_VECTOR3_H

//# generic vector3 class
//# vector3 is not aligned and operations in vector3
//# mostly avoid SSE or other optimization.

class _NexBaseAPI Vector3 : public nextar::AllocGeneral {
public:
	static const Vector3 XAxis;
	static const Vector3 YAxis;
	static const Vector3 ZAxis;
	static const Vector3 Origin;

	float x, y, z;

	// constructor,function,operators,etc

	//# construction and implicit conversions
	inline Vector3() : x(0), y(0), z(0) {}
	inline Vector3(float * const v) :
	x(v[0]), y(v[1]), z(v[2]) {
	}
	inline Vector3(float fx, float fy, float fz) :
	x(fx), y(fy), z(fz) {
	}
	inline Vector3(float v) :
	x(v), y(v), z(v) {
	}

	inline Vector3(const Vector3 & cp) :
	x(cp.x), y(cp.y), z(cp.z) {
	}
	inline Vector3(Vec3AF cp) {
		x = Vec3AGetX(cp);
		y = Vec3AGetY(cp);
		z = Vec3AGetZ(cp);
	}
	inline Vector3(const PolarSys & c) {
		*this = c;
	}

	//# operators
	//@ conversion
	//	inline operator float* () const;
	//	inline operator float* ();
	//@ assignment
	inline Vector3 & operator =(float val) {
		x = val;
		y = val;
		z = val;
		return *this;
	}
	//	inline Vector3& operator = (float*);
	inline Vector3 & operator =(const Vector3& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	inline Vector3 & operator =(const Vec3AF c) {
		x = Vec3AGetX(c);
		y = Vec3AGetY(c);
		z = Vec3AGetZ(c);
		return *this;
	}

	inline Vector3 & operator =(const PolarSys& pol);
	//@ access grant operator
	inline float operator [] (int32 i) const {
		NEX_ASSERT(i >= 0 && i < 3);
		return (&x)[i];
	}
	inline float & operator [] (int32 i) {
		NEX_ASSERT(i >= 0 && i < 3);
		return (&x)[i];
	}
	//@ unary operations
	inline Vector3 operator -() const {
		return Vector3(-x, -y, -z);
	}
	//@ binary operations
	inline Vector3 & operator *=(float w) {
		x *= w;
		y *= w;
		z *= w;
		return *this;
	}

	inline Vector3 & operator /=(float w) {
		NEX_ASSERT_FZERO(w);
		w = 1/w;
		x *= w;
		y *= w;
		z *= w;
		return *this;
	}

	inline Vector3 & operator +=(float w) {
		x += w;
		y += w;
		z += w;
		return *this;
	}
	inline Vector3 & operator -=(float w) {
		x -= w;
		y -= w;
		z -= w;
		return *this;
	}
	inline Vector3 & operator *=(const Vector3& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	inline Vector3 & operator /=(const Vector3& v) {
		NEX_ASSERT_FZERO(v.x);
		NEX_ASSERT_FZERO(v.y);
		NEX_ASSERT_FZERO(v.z);
		x /= v.x;

		y /= v.y;
		z /= v.z;
		return *this;
	}
	inline Vector3 & operator +=(const Vector3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	inline Vector3 & operator -=(const Vector3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	//# functions
	//@ compares with the input vector and stores
	//@ the elements that are greater than input
	inline void GetMaxElements(Vector3& store) const {
		if (x > store.x)
		store.x = x;
		if (y > store.y)
		store.y = y;
		if (z > store.z)
		store.z = z;

	}
	//@ compares with the input vector and stores
	//@ the elements that are lesser than input
	inline void GetMinElements(Vector3& store) const {
		if (x < store.x)
		store.x = x;
		if (y < store.y)
		store.y = y;
		if (z < store.z)
		store.z = z;

	}

	//@ compute absolute value
	inline void Abs() {
		x = Math::Abs(x);
		y = Math::Abs(y);
		z = Math::Abs(z);
	}
	//@ get the largest element index
	inline int32 LargestElement() const {
		return x > y ? (x > z ? 0 : 2) : (y > z ? 1 : 2);
	}

	//@ handy set
	inline void Set(float fx, float fy, float fz) {
		x = fx;
		y = fy;
		z = fz;
	}
	//@ lenght square
	inline float LengthSquare() const {
		return x*x + y*y + z*z;
	}
	//@ length
	inline float Length() const {
		return Math::Sqrt(LengthSquare());
	}

	//@ normalization
	inline void Normalize() {
		*this /= Length();
	}

	//@ incremental rotation about axis
	inline void RotateX(float sintheta, float costheta) {
		float oly = y;
		y = y * costheta - z * sintheta;
		z = oly * sintheta + z * costheta;
	}
	inline void RotateY(float sintheta, float costheta) {
		float olx = x;
		x = costheta * x + sintheta * z;
		z = -sintheta * olx + costheta * z;
	}
	inline void RotateZ(float sintheta, float costheta) {
		float olx = x;
		x = costheta * x - sintheta * y;
		y = sintheta * olx + costheta * y;
	}
	friend inline bool operator ==(const Vector3& v1, const Vector3& v2) {
		return (NEX_FLOAT_TOLERANCE_EQUAL(v1.x, v2.x, Math::ROUND_OFF)
				&& NEX_FLOAT_TOLERANCE_EQUAL(v1.y, v2.y, Math::ROUND_OFF)
				&& NEX_FLOAT_TOLERANCE_EQUAL(v1.z, v2.z, Math::ROUND_OFF)) != 0;
	}

	friend inline bool operator <(const Vector3& v1, const Vector3& v2) {
		return v1.x < v2.x && v1.y < v2.y && v1.z < v2.z;
	}

	friend inline bool operator >(const Vector3& v1, const Vector3& v2) {
		return v1.x > v2.x && v1.y > v2.y && v1.z > v2.z;
	}

	friend inline bool operator !=(const Vector3& v1, const Vector3& v2) {
		return !(v1 == v2);
	}

	friend inline Vector3 operator +(const Vector3& v1, const Vector3& v2) {
		return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}

	friend inline Vector3 operator -(const Vector3& v1, const Vector3& v2) {
		return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}

	friend inline Vector3 operator *(const Vector3& v1, const Vector3& v2) {
		return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	}

	friend inline Vector3 operator /(const Vector3& v1, const Vector3& v2) {
		return Vector3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
	}

	friend inline Vector3 operator *(const Vector3& v, float f) {
		return Vector3(v.x * f, v.y * f, v.z * f);
	}

	friend inline Vector3 operator /(const Vector3& v, float f) {
		NEX_ASSERT_FZERO(f);
		float recip = 1 / f;
		return Vector3(v.x * recip, v.y * recip, v.z * recip);
	}

	friend inline float DistanceSquare(const Vector3& v1, const Vector3& v2) {
		float x = v2.x - v1.x;
		float y = v2.y - v1.y;
		float z = v2.z - v1.z;
		return x * x + y * y + z * z;
	}

	friend inline float Distance(const Vector3& v1, const Vector3& v2) {
		return Math::Sqrt(DistanceSquare(v1, v2));
	}

	friend inline float Dot(const Vector3& v1, const Vector3& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	friend inline Vector3 Cross(const Vector3& v1, const Vector3& v2) {
		return Vector3((v1).y * (v2).z - (v1).z * (v2).y,
				(v1).z * (v2).x - (v1).x * (v2).z,
				(v1).x * (v2).y - (v1).y * (v2).x);
	}

};

#endif //NEXTAR_VECTOR3_H