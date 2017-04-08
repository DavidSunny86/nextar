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

	static inline type IsNan(pref v);
	static inline type IsInf(pref v);
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
class VecOp : public VecBaseOp<T, VecOp<U>> {
public:
};

template <typename U>
class VecOp<_Quad, U> {
public:
	typedef VecOp<U, U> impl;
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
	static inline type Set(base_type x, base_type y);
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
	static inline void SinCos(ref oSinVal, ref oCosVal, base_type val);
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

template <>
class VecOp<_Vec4, _Vec4> : public VecOp<_Quad, _Vec4> {
public:
	static inline type Mul(pref v, TraitsMat4x4::pref m);
};

template <>
class VecOp<_Vec3A, _Vec3A> : public VecOp<_Quad, _Vec3A> {
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
class VecOp<_Plane, _Plane> : public VecOp<_Quad, _Plane> {
public:
};

template <>
class VecOp<_Quat, _Quat> : public VecOp<_Quad, _Quat> {
public:
	static inline type FromMat4x4(TraitsMat4x4::pref m);
	static inline type FromMat3x4(TraitsMat3x4::pref m);
};

template <>
class VecOp<_AxisAngle, _AxisAngle> : public VecOp<_Quad, _AxisAngle> {
public:
	static inline type FromMat4x4(TraitsMat4x4::pref m);
	static inline type FromMat3x4(TraitsMat3x4::pref m);
};

template <>
class VecOp<_EulerAngles, _EulerAngles> : public VecOp<_Quad, _EulerAngles> {
public:
	static inline type FromMat4x4(TraitsMat4x4::pref m);
	static inline type FromMat3x4(TraitsMat3x4::pref m);
};

template <>
class VecOp<_Vec3, _Vec3> : public VecBaseOp<_Vec3, VecOp<_Vec3, _Vec3>> {
public:
	static inline type Cross(pref q1, pref q2);
};

template <typename T>
class VecOpImpl : public VecOp<T, T> {
public:
};

typedef VecOpImpl<_Quad> QuadOp;
typedef VecOpImpl<_Vec2> Vec2Op;
typedef VecOpImpl<_Vec3> Vec3Op;
typedef VecOpImpl<_Vec4> Vec4Op;
typedef VecOpImpl<_Plane> PlaneOp;
typedef VecOpImpl<_Quat> QuatOp;
typedef VecOpImpl<_AxisAngle> AxisAngleOp;
typedef VecOpImpl<_EulerAngles> EulerAnglesOp;
typedef VecOpImpl<_PolarCoord> PolarCoordOp;

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

	static inline row_type operator ()(pref m, uint32 i) const;
	static inline base_type operator ()(pref m, uint32 i, uint32 j) const;
};


template <typename T>
class MatOp : public MatBaseOp<T> {
public:
};


typedef MatOp<_AxisAlignedBox> AABoxOp;
typedef MatOp<_Matrix3x4> Mat3x4Op;
typedef MatOp<_Matrix4x4> Mat4x4Op;
typedef MatOp<_Rect> RectOp;

}
}

