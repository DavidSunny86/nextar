#ifndef NEXTAR_MATH_SSETYPES_H
#define NEXTAR_MATH_SSETYPES_H

#ifndef NEXTAR_MATHTYPES_H
#error Do not include this file directly, Include  NexMath.h
#endif

/**
 @remarks Core types
 **/
//!! typedefs
//!! basic type which defines all operations
typedef __m128 Quad;

typedef Quad Vector3A;
typedef Quad Vector4A;
typedef Quad Quaternion;
typedef Quad Plane;

NEX_ALIGNED_STRUCT_BEGIN(16, Matrix3x4) {

	union {
		struct {
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
		};
		struct {
			Vector3A x;
			Vector3A y;
			Vector3A z;
		};
		float m[12];
		Vector3A r[3];
	};

	/* functions */
	_NexInline Matrix3x4() {
	}
	_NexInline const float operator [] (size_t i) const {
		return m[i];
	}
	_NexInline float& operator [] (size_t i) {
		return m[i];
	}
	_NexInline Matrix3x4 & operator =(const Matrix3x4&);
}
NEX_ALIGNED_STRUCT_END(16);

NEX_ALIGNED_STRUCT_BEGIN(16, Matrix4x4) {
	static _NexExport const Matrix4x4 IdentityMatrix;

	union {
		struct {
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
		float m[16];
		Vector4A r[4];
	};

	/* functions */
	_NexInline Matrix4x4() {
	}
	_NexInline Matrix4x4(
			const Matrix3x4&,
			const Vector4A&);
	_NexInline Matrix4x4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);
	_NexInline const float operator [] (size_t i) const {
		return m[i];
	}
	_NexInline float& operator [] (size_t i) {
		return m[i];
	}
	_NexInline Matrix4x4 & operator =(const Matrix4x4&);
}
NEX_ALIGNED_STRUCT_END(16);

typedef const Quad QuadPF; // fast param
typedef const Quad& QuadPR; // ref param

typedef QuadPF Vec3AF;
typedef QuadPR Vec3AR;

typedef QuadPF Vec4AF;
typedef QuadPR Vec4AR;

typedef QuadPF QuatF;
typedef QuadPR QuatR;

typedef QuadPF PlaneF;
typedef QuadPR PlaneR;

typedef const Matrix4x4& Mat4x4F;
typedef const Matrix4x4& Mat4x4R;

typedef const Matrix3x4& Mat3x4F;
typedef const Matrix3x4& Mat3x4R;

#include "mathtypes/sse/NexSSEConstants.h"

/**
 @remarks Quad related functions
 **/

/** @remarks Load unaligned floats **/
_NexInline Quad QuadLoad3UF(const float* vals);
_NexInline Quad QuadLoad4UF(const float* vals);

/** @remarks Load aligned floats **/
_NexInline Quad QuadLoad3AF(const float* vals);
_NexInline Quad QuadLoad4AF(const float* vals);

/** @remarks Set elements **/
_NexInline Quad QuadSet(float x, float y, float z, float w);
_NexInline Quad QuadZero();
_NexInline Quad QuadSetX(QuadPF q, float val);
_NexInline Quad QuadSetY(QuadPF q, float val);
_NexInline Quad QuadSetZ(QuadPF q, float val);

/** @remarks Get elements **/
_NexInline float QuadGetByIdx(QuadPF q, size_t idx);
_NexInline float QuadGetX(QuadPF q);
_NexInline float QuadGetY(QuadPF q);
_NexInline float QuadGetZ(QuadPF q);
_NexInline float QuadGetW(QuadPF q);

/** @remarks Replicat X component all over **/
_NexInline Quad QuadSplatX(QuadPF q);
/** @remarks Replicat Y component all over **/
_NexInline Quad QuadSplatY(QuadPF q);
/** @remarks Replicat Z component all over **/
_NexInline Quad QuadSplatZ(QuadPF q);
/** @remarks Replicat W component all over **/
_NexInline Quad QuadSplatW(QuadPF q);
/** @remarks Replicat val all over **/
_NexInline Quad QuadReplicate(float val);

/** @remarks Abs elements **/
_NexInline Quad QuadAbs(QuadPF q);
/** @remarks Negate elements **/
_NexInline Quad QuadNegate(QuadPF q);
/** @remarks Negate elements **/
_NexInline Quad QuadAdd(QuadPF a, QuadPF b);
/** @remarks Negate elements **/
_NexInline Quad QuadSub(QuadPF a, QuadPF b);
/** @remarks Negate elements **/
_NexInline Quad QuadMul(QuadPF a, QuadPF b);
/** @remarks Negate elements **/
_NexInline Quad QuadDiv(QuadPF a, QuadPF b);
/** @remarks Scalar multiply **/
_NexInline Quad QuadMulScalar(QuadPF q, float val);
/** @remarks Multiply and Add: a*b+c **/
_NexInline Quad QuadMulAdd(QuadPF a, QuadPF b, QuadPF c);
/** @remarks Splat Sin and Cos value of 'val' **/
_NexInline void QuadSinCos(Quad& vsinval, Quad& vcosval, const float val);
/** @reamarks Calculates reciprocal square root */
_NexInline Quad QuadInvSqrt(QuadPF);
/* Quad select */
_NexInline Quad QuadSelect(QuadPF q1, QuadPF q2);

#define Mat4x4Row(m, i) (m).r[i]

/**
 * @remarks Vec3 functions
 **/
#define Vec3AZero       QuadZero
#define Vec3AGetX		QuadGetX
#define Vec3AGetY		QuadGetY
#define Vec3AGetZ		QuadGetZ
#define Vec3AAbs		QuadAbs
#define Vec3ANegate		QuadNegate
#define Vec3AMulScalar          QuadMulScalar
#define Vec3AAdd		QuadAdd
#define Vec3ASub		QuadSub
#define Vec3AMulAdd		QuadMulAdd
#define Vec3AMulMat4x4  Mat4x4TransVec3
#define Vec3AMul        QuadMul

/**
 * @remarks Vec3 functions. Rename those already defined.
 **/
#define Vec4AZero               QuadZero
#define Vec4ASet                QuadSet
#define Vec4AGetX		QuadGetX
#define Vec4AGetY		QuadGetY
#define Vec4AGetZ		QuadGetZ
#define Vec4AGetW		QuadGetW
#define Vec4AAbs		QuadAbs
#define Vec4ANegate     QuadNegate
#define Vec4AAdd		QuadAdd
#define Vec4ASub		QuadSub
#define Vec4AMulAdd		QuadMulAdd
#define Vec4AReplicate          QuadReplicate
#define Vec4AGreaterAny         QuadGreaterAny
#define Vec4AGreaterAll         QuadGreaterAll
#define Vec4ALesserAny          QuadLesserAny
#define Vec4ALesserAll          QuadLesserAll

#define QuatSet QuadSet
#define QuatGetX QuadGetX
#define QuatGetY QuadGetY
#define QuatGetZ QuadGetZ
#define QuatGetW QuadGetW

#endif //NEXTAR_MATH_SSETYPES_H
