#ifndef NEXTAR_MATH_CTYPES_H
#define NEXTAR_MATH_CTYPES_H

//#ifndef NEXTAR_MATHTYPES_H
//#error Do not include this file directly, Include  NexMath.h
//#endif

/**
 @remarks Core types
 **/
//!! typedefs
//!! basic type which defines all operations
struct Quad {
	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		float v[4];
	};

	_NexInline Quad() {
	}

	_NexInline Quad(const Quad & q) :
			x(q.x), y(q.y), z(q.z), w(q.w) {
	}

	_NexInline Quad(float vx, float vy, float vz) :
			x(vx), y(vy), z(vz), w(0) {
	}

	_NexInline Quad(float vx, float vy, float vz, float vw) :
			x(vx), y(vy), z(vz), w(vw) {
	}

	void Load(float val) {
		x = y = z = w = val;
	}

	void Load3(const float * vals) {
		x = vals[0];
		y = vals[1];
		z = vals[2];
	}

	void Store3(float * vals) const {
		vals[0] = x;
		vals[1] = y;
		vals[2] = z;
	}

	void Load4(const float * vals) {
		x = vals[0];
		y = vals[1];
		z = vals[2];
		w = vals[3];
	}

	void Store4(float * vals) const {
		vals[0] = x;
		vals[1] = y;
		vals[2] = z;
		vals[3] = w;
	}

	Quad SplatX() const {
		return Quad(x, x, x, x);
	}

	Quad SplatY() const {
		return Quad(y, y, y, y);
	}

	Quad SplatZ() const {
		return Quad(z, z, z, z);
	}

	Quad SplatW() const {
		return Quad(w, w, w, w);
	}

};

typedef Quad Vector3A;
typedef Quad Vector4A;
typedef Quad Quaternion;
typedef Quad Plane;

struct Matrix3x4 {

	union {

		struct {
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
		};

		float m[12];
	};

	/* functions */
	_NexInline Matrix3x4() {
	}

	_NexInline
	Vector3A & Row(int i) {
		return *(Vector3A*) (&m[i * 4]);
	}

	_NexInline
	const Vector3A & Row(int i) const {
		return *(Vector3A*) (&m[i * 4]);
	}

	_NexInline
	const float operator [](int i) const {
		return m[i];
	}

	_NexInline
	float & operator [](int i) {
		return m[i];
	}

	_NexInline
	float& operator ()(int i, int j) {
		return m[i * 4 + j];
	}

	_NexInline
	float operator ()(int i, int j) const {
		return m[i * 4 + j];
	}

	_NexInline
	Matrix3x4 & operator =(const Matrix3x4&);

};

struct Matrix4x4 {
	static _NexExport const Matrix4x4 IdentityMatrix;

	union {

		struct {
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
		float m[16];
	};

	/* functions */
	_NexInline Matrix4x4() {
	}
	_NexInline Matrix4x4(const Matrix3x4&, const Vector4A&);
	_NexInline Matrix4x4(float m00, float m01, float m02, float m03, float m10, float m11,
			float m12, float m13, float m20, float m21, float m22, float m23, float m30,
			float m31, float m32, float m33);

	_NexInline
	const float operator [](int i) const {
		return m[i];
	}

	_NexInline
	float & operator [](int i) {
		return m[i];
	}

	_NexInline
	Vector3A & Row(int i) {
		return *(Vector3A*) (&m[i * 4]);
	}

	_NexInline
	const Vector3A & Row(int i) const {
		return *(Vector3A*) (&m[i * 4]);
	}

	_NexInline
	float& operator ()(int i, int j) {
		return m[i * 4 + j];
	}

	_NexInline
	float operator ()(int i, int j) const {
		return m[i * 4 + j];
	}

	_NexInline
	Matrix4x4 & operator =(const Matrix4x4&);
};

typedef const Quad& QuadPF; // fast param
typedef const Quad& QuadPR; // ref param

typedef QuadPF Vec3AF;
typedef QuadPR Vec3AR;

typedef QuadPF Vec4AF;
typedef QuadPR Vec4AR;

typedef QuadPF QuatF;
typedef QuadPR QuatR;

typedef QuadPF PlaneF;
typedef QuadPR PlaneR;

typedef const Matrix3x4& Mat3x4F;
typedef const Matrix3x4& Mat3x4R;

typedef const Matrix4x4& Mat4x4F;
typedef const Matrix4x4& Mat4x4R;

/** Inlined functions **/
_NexInline float QuadGetX(QuadPF q) {
	return q.x;
}

_NexInline float QuadGetY(QuadPF q) {
	return q.y;
}

_NexInline float QuadGetZ(QuadPF q) {
	return q.z;
}

_NexInline float QuadGetW(QuadPF q) {
	return q.w;
}

/** definitions come here */
#define Vec4AGetX		QuadGetX
#define Vec4AGetY		QuadGetY
#define Vec4AGetZ		QuadGetZ
#define Vec4AGetW		QuadGetW
/**
 * @remarks Vec3 functions
 **/
#define Vec3AGetX		QuadGetX
#define Vec3AGetY		QuadGetY
#define Vec3AGetZ		QuadGetZ
#define Vec3AMulMat4x4  Mat4x4TransVec3A

#define PlaneNormalize		Vec3ANormalize
#define PlaneNormalDotVec3A     Vec3ADot
#define PlaneAbsNormal          QuadAbs

#define QuatGetX QuadGetX
#define QuatGetY QuadGetY
#define QuatGetZ QuadGetZ
#define QuatGetW QuadGetW
#define QuatNormalize Vec4ANormalize
#define QuatDot Vec4ADot
#define QuatGreaterAny Vec4AGreaterAny
#define QuatGreaterAll Vec4AGreaterAll
#define QuatLesserAny Vec4ALesserAny
#define QuatLesserAll Vec4ALesserAll

#define Mat4x4Row(m, i) (m).Row(i)

#endif //NEXTAR_MATH_SSETYPES_H
