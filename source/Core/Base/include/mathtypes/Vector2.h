#ifndef NEXTAR_VECTOR2_H
#define NEXTAR_VECTOR2_H

/** 
 * Generic vector class Vector2 is aligned and uses mostly
 * SSE or other optimization.
 * This class represents a 2d vector.
 */
class _NexBaseExport Vector2 : public nextar::AllocGeneral {
public:
	float x, y;
	static const Vector2 Origin;

	// constructor,function,operators,etc

	inline Vector2() {
	}

	inline Vector2(const float v) : x(v), y(v) {
	}

	inline Vector2(const float * v) : x(v[0]), y(v[1]) {
	}

	inline Vector2(const float fx, const float fy) : x(fx), y(fy) {
	}

	inline Vector2(const Vector2 & cp) : x(cp.x), y(cp.y) {
	}

	inline Vector2(const Vector3 & cp) : x(cp.x / cp.z), y(cp.y / cp.z) {
	}

	// operator
	// assignment

	inline Vector2 & operator =(const float val) {
		x = y = val;
		return *this;
	}

	inline Vector2 & operator =(const Vector2 & val) {
		x = val.x;
		y = val.y;
		return *this;
	}

	inline Vector2 & operator =(const Vector3 & val) {
		x = val.x / val.z;
		y = val.y / val.z;
		return *this;
	}

	// access grant operator

	inline const float operator [] (int32 i) const {
		NEX_ASSERT(i >= 0 && i < 2);
		return (&x)[i];
	}

	inline float & operator [] (int32 i) {
		NEX_ASSERT(i >= 0 && i < 2);
		return (&x)[i];
	}

	// unary operations

	inline Vector2 operator -() const {
		return Vector2(-x, -y);
	}

	// binary operations

	inline Vector2 & operator *=(const float val) {
		x *= val;
		y *= val;
		return *this;
	}

	inline Vector2 & operator /=(const float val) {
		*this *= 1 / val;
		return *this;
	}

	inline Vector2 & operator +=(const float val) {
		x += val;
		y += val;
		return *this;
	}

	inline Vector2 & operator -=(const float val) {
		x -= val;
		y -= val;
		return *this;
	}

	inline Vector2 & operator *=(const Vector2 & v) {
		x *= v.x;
		y *= v.y;
		return *this;
	}

	inline Vector2 & operator /=(const Vector2 & v) {
		NEX_ASSERT_FZERO(v.x);
		NEX_ASSERT_FZERO(v.y);
		x /= v.x;
		y /= v.y;
		return *this;
	}

	inline Vector2 & operator +=(const Vector2 & v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	inline Vector2 & operator -=(const Vector2 & v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}

	// compares with the input vector and stores
	// the elements that are greater than input

	inline void GetMaxElements(Vector2 & store) const {
		if (x > store.x)
		store.x = x;
		if (y > store.y)
		store.y = y;
	}

	// compares with the input vector and stores
	// the elements that are lesser than input

	inline void GetMinElements(Vector2& store) const {
		if (x < store.x)
		store.x = x;
		if (y < store.y)
		store.y = y;

	}

	// compute absolute value

	inline void Abs() {
		x = Math::Abs(x);
		y = Math::Abs(y);
	}

	// get the largest element index

	inline int32 LargestElement() const {
		return x > y ? 0 : 1;
	}

	// handy set

	inline void Set(const float vx, const float vy) {
		x = vx;
		y = vy;
	}

	// length

	inline float Length() const {
		return Math::Sqrt(x * x + y * y);
	}

	// lenght square

	inline float LengthSquare() const {
		return (x * x + y * y);
	}

	// normalization

	inline void Normalize() {
		float v = Length();
		x /= v;
		y /= v;
	}

	// friend functions and operators
	//# friend operators

	friend inline bool operator ==(const Vector2& v1, const Vector2 & v2) {
		return (NEX_FLOAT_TOLERANCE_EQUAL(v1.x, v2.x, Math::ROUND_OFF) &&
				NEX_FLOAT_TOLERANCE_EQUAL(v1.y, v2.y, Math::ROUND_OFF)) != 0;
	}

	friend inline bool operator !=(const Vector2& v1, const Vector2 & v2) {
		return !(v1 == v2);
	}

	friend inline Vector2 operator +(const Vector2& v1, const Vector2 & v2) {
		return Vector2(v1.x + v2.x, v1.y + v2.y);
	}

	friend inline Vector2 operator -(const Vector2& v1, const Vector2 & v2) {
		return Vector2(v1.x - v2.x, v1.y - v2.y);
	}

	friend inline Vector2 operator *(const Vector2& v1, const Vector2 & v2) {
		return Vector2(v1.x * v2.x, v1.y * v2.y);
	}

	friend inline Vector2 operator /(const Vector2& v1, const Vector2 & v2) {
		return Vector2(v1.x / v2.x, v1.y / v2.y);
	}

	friend inline Vector2 operator *(const Vector2& v1, const float val) {
		return Vector2(v1.x*val, v1.y * val);
	}

	friend inline Vector2 operator /(const Vector2& v1, const float val) {
		return v1 * (1 / val);
	}

	// friend functions

	friend inline float DistanceSquare(const Vector2& v1, const Vector2 & v2) {
		float x = v2.x - v1.x;
		float y = v2.y - v1.y;
		return x * x + y*y;
	}

	friend inline float Distance(const Vector2& a, const Vector2 & b) {
		return Math::Sqrt(DistanceSquare(a,b));
	}

	friend inline float Dot(const Vector2& v1, const Vector2 & v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}
};

#endif //NEXTAR_VECTOR2_H
