#pragma once

#include <NexBase.h>
#include <PooledAllocator.h>

namespace nextar {
namespace Math {

//=========================================================
// Vector operations
//=========================================================
template <typename T, typename impl>
class VecBaseOp {
public:

	typedef Traits<T> traits;
	typedef typename traits::type type;
	typedef typename traits::ref ref;
	typedef typename traits::pref pref;
	typedef typename traits::cref cref;
	typedef typename traits::base_type base_type;
	typedef typename traits::row_type row_type;
	typedef float float_type;

	enum {
		_count = traits::_count
	};

	static inline bool IsNan(pref v);
	static inline bool IsInf(pref v);
	static inline type Set(base_type v);
	static inline type Set(base_type x, base_type y);
	static inline type Set(base_type x, base_type y, base_type z);
	static inline type Set(base_type x, base_type y, base_type z, base_type w);
	static inline type Set(const base_type* v);
	static inline type Zero();
	static inline base_type Get(pref v, uint32 i);
	static inline type Abs(pref v);
	static inline type Negate(pref v);
	static inline type Add(pref a, pref b);
	static inline type Sub(pref a, pref b);
	static inline type Mul(pref a, pref b);
	static inline type Mul(pref a, base_type b);
	static inline type Mul(base_type b, pref a);
	static inline type Div(pref a, pref b);
	static inline type Madd(pref v, pref m, pref a);
	static inline base_type Hadd(pref q1);
	static inline bool GreaterAll(pref q1, pref q2);
	static inline bool GreaterAny(pref q1, pref q2);
	static inline bool LesserAll(pref q1, pref q2);
	static inline bool LesserAny(pref q1, pref q2);
	static inline base_type Dot(pref q1, pref q2);
	static inline float_type SqLength(pref c1);
	static inline float_type Length(pref c1);
	static inline float_type Distance(pref vec1, pref vec2);
	static inline float_type SqDistance(pref vec1, pref vec2);
};

template <typename T, typename U=T>
class _VecOp : public VecBaseOp<T, _VecOp<T, U>> {
public:
};

template <typename U>
class _VecOp<_Quad, U> {
public:
	typedef _VecOp<U, U> impl;
	typedef Traits<U> traits;
	typedef typename traits::type type;
	typedef typename traits::ref ref;
	typedef typename traits::pref pref;
	typedef typename traits::cref cref;
	typedef typename traits::base_type base_type;
	typedef typename traits::row_type row_type;
	typedef float float_type;

	static inline type IsNan(pref v);
	static inline type IsInf(pref v);
	static inline type Set(base_type v);
	static inline type Set(base_type x, base_type y, base_type z);
	static inline type Set(base_type x, base_type y, base_type z, base_type w);
	static inline type Set(const base_type* v);
	static inline type Zero();
	static inline base_type Get(pref v, uint32 i);
	static inline base_type GetX(pref v);
	static inline base_type GetY(pref v);
	static inline base_type GetZ(pref v);
	static inline base_type GetW(pref v);
	static inline type SetX(pref v, base_type x);
	static inline type SetY(pref v, base_type y);
	static inline type SetZ(pref v, base_type z);
	static inline type SetW(pref v, base_type w);
	static inline type SplatX(pref v);
	static inline type SplatY(pref v);
	static inline type SplatZ(pref v);
	static inline type SplatW(pref v);
	static inline type Abs(pref v);
	static inline type Negate(pref v);
	static inline type Add(pref a, pref b);
	static inline type Sub(pref a, pref b);
	static inline type Mul(pref a, pref b);
	static inline type Mul(pref a, base_type b);
	static inline type Mul(base_type b, pref a);
	static inline type Div(pref a, pref b);
	static inline type Madd(pref v, pref m, pref a);
	static inline base_type Hadd(pref q1);
	static inline type InvSqrt(pref ival);
	static inline type Select(pref v1, pref v2, pref control);
	static inline bool GreaterAll(pref q1, pref q2);
	static inline bool GreaterAny(pref q1, pref q2);
	static inline bool LesserAll(pref q1, pref q2);
	static inline bool LesserAny(pref q1, pref q2);
	static inline type VDot(pref q1, pref q2);
	static inline base_type Dot(pref q1, pref q2);
	static inline type Normalize(pref q);
	static inline type Lerp(pref c1, pref c2, float_type t);
	static inline float_type Length(pref c1);
	static inline float_type SqLength(pref c1);
	static inline float_type Distance(pref vec1, pref vec2);
	static inline float_type SqDistance(pref vec1, pref vec2);
};

template <typename T>
class VecOp : public _VecOp<T, T> {
public:
};

template <>
class _VecOp<_Vec4, _Vec4> : public _VecOp<_Quad, _Vec4> {
public:
	static inline type Mul(pref v, TraitsMat4x4::pref m);
};

template <>
class _VecOp<_Vec3A, _Vec3A> : public _VecOp<_Quad, _Vec3A> {
public:
	static inline type VDot(pref q1, pref q2);
	static inline base_type Dot(pref q1, pref q2);
	static inline type Normalize(pref q);
	static inline type Cross(pref q1, pref q2);
	static inline bool GreaterAll(pref q1, pref q2);
	static inline bool GreaterAny(pref q1, pref q2);
	static inline bool LesserAll(pref q1, pref q2);
	static inline bool LesserAny(pref q1, pref q2);
	static inline type Mul(pref v, TraitsMat4x4::pref m);
};

template <>
class _VecOp<_Plane, _Plane> : public _VecOp<_Quad, _Plane> {
public:
	static inline type Normalize(pref q);
	static inline float_type Dot(pref plane, TraitsVec3A::pref q);
	// dot with the normal
	static inline float_type DotNormal(pref plane, TraitsVec3A::pref q);
	// abs with the normal
	static inline type AbsNormal(pref plane);
};

template <>
class _VecOp<_Quat, _Quat> : public _VecOp<_Quad, _Quat> {
public:
	static inline type Identity();
	static inline type FromAxisAngle(TraitsVec3::pref v, float_type ang);
	static inline type FromAxisAngle(TraitsAxisAngle::pref v);
	static inline type FromMat4x4(TraitsMat4x4::pref m);
	static inline type FromMat3x4(TraitsMat3x4::pref m);
	static inline type Mul(pref q1, pref q2);
	static inline TraitsVec3A::type Transform(pref q1, TraitsVec3A::pref q2);
	static inline TraitsVec3A::type TransformBounds(pref q1, TraitsVec3A::pref extends);
	static inline type Slerp(pref q1, pref q2, float_type t);
	static inline type Lerp(pref q1, pref q2, float_type t);
	static inline type Normalize(pref q1);
	static inline type Inverse(pref q);
};

template <>
class _VecOp<_AxisAngle, _AxisAngle> : public _VecOp<_Quad, _AxisAngle> {
public:
};

template <>
class _VecOp<_EulerAngles, _EulerAngles> : public _VecOp<_Quad, _EulerAngles> {
public:
	static inline type Canonize(pref m);
	static inline type FromQuat(TraitsQuat::pref m);
	static inline type FromInvQuat(TraitsQuat::pref m);
	static inline type FromMat4x4(TraitsMat4x4::pref m);
	static inline type FromMat3x4(TraitsMat3x4::pref m);
};

template <>
class _VecOp<_Vec3, _Vec3> : public VecBaseOp<_Vec3, _VecOp<_Vec3, _Vec3>> {
public:
	static inline type Cross(pref q1, pref q2);
};

//=========================================================
// Matrix operations
//=========================================================
template <typename T>
class MatBaseOp {
public:
	typedef MatOp<T> matop;
	typedef Traits<T> traits;
	typedef traits::type type;
	typedef traits::ref ref;
	typedef traits::pref pref;
	typedef traits::cref cref;
	typedef traits::base_type base_type;
	typedef traits::row_type row_type;
	typedef traits::float_type float_type;

	enum {
		_count = traits::_count,
		_rows = traits::_rows,
		_columns = traits::_columns,
	};

	static inline row_type Row(pref m, uint32 i) const;
	static inline base_type Get(pref m, uint32 i, uint32 j) const;
};


template <typename T>
class MatOp : public MatBaseOp<T> {
public:
};

template <typename T>
class _Mat4Op : public MatBaseOp<T> 	{
public:
	// @brief Create a matrix from vector mapping that
	// can rotate the vector axis1 to axis2 when post multiplied to axis1.
	static inline type FromVectorMapping(TraitsVec3::pref v1, TraitsVec3::pref v2);
	// @brief rotate vector in place
	static inline void Rotate(pref m,
		TraitsVec3::type* iStream, uint32 inStride, uint32 count);
	// @brief rotate vector
	static inline TraitsVec3A::type Rotate(pref m, TraitsVec3A::pref v);
	// @brief Create a rotation matrix from quaternion
	static inline type FromQuat(TraitsQuat::pref rot);
	// @brief Create a rotation matrix from quaternion
	static inline type FromRot(TraitsQuat::pref rot);
	// @brief Returns a look at matrix based on a look at vector and up position 
	static inline type FromViewAndUp(TraitsVec3A::pref viewDir, TraitsVec3A::pref up);
};

template <>
class MatOp<_Matrix4x4> : public _Mat4Op<_Matrix4x4> {
public:
	// @brief Full matrix multiplication
	static inline type Mul(pref m1, pref m2) const;
	// @brief Transform vertices assuming orthogonal matrix
	static inline void TransformOrtho(pref m,
		const TraitsVec3::type* iStream, uint32 inStride, uint32 count,
		TraitsVec4::type* oStream, uint32 outStride);
	// @brief Transform vertices assuming orthogonal matrix
	static inline void TransformOrtho(pref m,
		const TraitsVec3::type* iStream, uint32 inStride, uint32 count,
		TraitsVec3::type* oStream, uint32 outStride);
	// @brief Transform vertices in place, assuming orthogonal matrix
	static inline void TransformOrtho(pref m,
		TraitsVec3::type* iStream, uint32 inStride, uint32 count);
	// @brief Transform vertices and project the w coord as 1.0.
	static inline void Transform(pref m,
		const TraitsVec3::type* iStream, uint32 inStride, uint32 count,
		TraitsVec3::type* oStream, uint32 outStride);
	// @brief Transform vertices assuming orthogonal matrix
	static inline TraitsVec3A::type TransformOrtho(pref m, TraitsVec3A::pref v);
	// @brief Transform vertices and project the w coord as 1.0.
	static inline TraitsVec3A::type Transform(pref m, TraitsVec3A::pref v);
	// @brief Special transform for AABB bound extends. 
	static inline TraitsVec3A::type TransformBounds(pref m, TraitsVec3A::pref extends);
	// @brief Special transform for AABB min and max
	static inline TraitsAABox::type TransformAABox(pref m, TraitsAABox::pref v);

	static inline type FromScaleRotPos(float_type scale, TraitsQuat::pref rot, TraitsVec3A::pref pos);
	static inline type FromScale(TraitsVec3A::pref scale);
	static inline type FromPos(TraitsVec3A::pref pos);
	// @brief Create a view matrix from camera world matrix
	static inline type FromWorldToView(pref m);
	// @brief Matrix4x4 Creates a camera look at matrix 
	static inline type FromLookAt(TraitsVec3A::pref eye, TraitsVec3A::pref lookAt, TraitsVec3A::pref up);
	// @brief Orthogonal Projection matrix 
	static inline type FromOrthoProjection(float_type width, float_type height, float_type nearPlane,
		float_type farPlane);
	// @brief Perspective Projection matrix 
	static inline type FromPerspectiveProjection(float_type fieldOfView, float_type aspectRatio,
		float_type nearPlane, float_type farPlane);
	// @brief Scale
	static inline type Scale(pref m, float_type amount);
	static inline type Transpose(pref m);
	static inline type Inverse(pref m);
	// @brief Inverse for orthogonal matrix
	static inline type InverseOrtho(pref m);
};

template <>
class MatOp<_Matrix3x4> : public _Mat4Op<_Matrix3x4> {
public:
	static inline type Transpose(pref m);
};

template <>
class MatOp<_AxisAlignedBox> : public MatBaseOp<_AxisAlignedBox> {
public:
	static inline bool IsValid(pref box);
	static inline TraitsVec3A::type GetCenter(pref box);
	static inline TraitsVec3A::type GetSize(pref box);
	static inline TraitsVec3A::type GetPoint(pref box, unsigned int i);
	static inline type Union(pref box, TraitsVec3A::pref point);
	static inline type Union(pref box, pref other);
};

typedef VecOp<_Quad> QuadOp;
typedef VecOp<_Vec2> Vec2Op;
typedef VecOp<_Vec3> Vec3Op;
typedef VecOp<_Vec3A> Vec3AOp;
typedef VecOp<_Vec4> Vec4Op;
typedef VecOp<_Plane> PlaneOp;
typedef VecOp<_Quat> QuatOp;
typedef VecOp<_AxisAngle> AxisAngleOp;
typedef VecOp<_EulerAngles> EulerAnglesOp;
typedef VecOp<_PolarCoord> PolarCoordOp;

typedef MatOp<_AxisAlignedBox> AABoxOp;
typedef MatOp<_Matrix3x4> Mat3x4Op;
typedef MatOp<_Matrix4x4> Mat4Op;
typedef Mat4Op Mat4x4Op;
typedef MatOp<_Rect> RectOp;



template<typename T>
inline type _Mat4Op<T>::FromRot(TraitsQuat::pref rot) {
	return type();
}

template<typename T>
inline type _Mat4Op<T>::FromViewAndUp(TraitsVec3A::pref viewDir, TraitsVec3A::pref up) {
	return type();
}

}
}

