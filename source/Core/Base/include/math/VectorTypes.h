/*
 * VectorTypes.h
 *
 *  Created on: Feb 9, 2014
 *      Author: obhi
 */

#ifndef VECTORTYPES_H_
#define VECTORTYPES_H_

#include <NexBase.h>
#include <PooledAllocator.h>

namespace nextar {
namespace Math {

struct _Matrix3x4;
struct _Matrix4x4;
struct _AxisAlignedBox;
struct _PolarCoord;
struct _AxisAngle;
struct _EulerAngles;
struct _Rect;

#if NEX_VECTOR_MATH_TYPE_IS_SSE
typedef __m128 _Quad;
#else
struct alignas(16) _Quad {
	typedef _Vec2 type;
	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		float v[4];
	};
};
#endif

typedef _Quad Quad;

struct _Vec2 {
	typedef _Vec2 type; 
	union { 
		struct { 
			float x, y; 
		};
		float v[2]; 
	};
};

struct _Vec3 { 
	typedef _Vec3 type; 
	union {
		struct {
			float x, y, z;
		};
		float v[3];
	};
};

struct _Vec3A { typedef _Quad type; };
struct _Vec4 { typedef _Quad type; };
struct _Plane { typedef _Quad type; };
struct _Quat { typedef _Quad type; };
struct _AxisAngle { typedef _Quad type; };
struct _PolarCoord { typedef _Vec2 type; };
struct _EulerAngles { typedef _Vec3 type; };
#ifdef _MSC_VER
typedef __declspec(align(16)) _Matrix3x4 Matrix3x4;
typedef __declspec(align(16)) _Matrix4x4 Matrix4x4;
typedef __declspec(align(16)) _AxisAlignedBox AxisAlignedBox;
#else
using Matrix3x4 alignas(16) = _Matrix3x4;
using Matrix4x4 alignas(16) = _Matrix4x4;
using AxisAlignedBox alignas(16) = _AxisAlignedBox;
#endif


typedef _Vec2::type Vector2;
typedef _Vec3::type Vector3;
typedef _Vec3A::type Vector3A;
typedef _Vec4::type Vector4;
typedef _Plane::type Plane;
typedef _Quat::type Quaternion;
typedef _AxisAngle::type AxisAngle;
typedef _EulerAngles::type EulerAngles;
typedef _PolarCoord::type PolarCoord;

typedef AllocMathPool< _Matrix3x4, NEX_MATRIX_POOL_NUM_PER_BLOCK > AllocMatrix3x4;
typedef AllocMathPool< _Matrix4x4, NEX_MATRIX_POOL_NUM_PER_BLOCK > AllocMatrix4x4;
typedef AllocMathPool< _AxisAlignedBox, NEX_MATRIX_POOL_NUM_PER_BLOCK > AllocAABox;
typedef PooledAllocator< _AxisAlignedBox, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE > AllocatorAABox;
typedef PooledAllocator< _Matrix3x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE > AllocatorMatrix3x4;
typedef PooledAllocator< _Matrix4x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE > AllocatorMatrix4x4;
typedef PooledAllocator<_Quad, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE> AllocatorQuad;
template <const uint32 n>
struct _IVecN {
	typedef _IVecN<n> type;
	std::array<int32, n> v;
};

typedef _IVecN<2> IVector2;
typedef _IVecN<3> IVector3;
typedef _IVecN<4> IVector4;

struct alignas(16) _Matrix3x4 : public AllocMatrix3x4 {
	typedef Matrix3x4 type;

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
};

struct alignas(16) _Matrix4x4 : public AllocMatrix4x4 {
	typedef Matrix4x4 type;

	static _NexBaseAPI const Matrix4x4 IdentityMatrix;

	union {
		struct {
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
		float m[16];
		Vector3A r[4];
	};

};

struct _Rect {
	typedef _Rect type;
	union {
		struct {
			Vector2 leftTop;
			Vector2 rightBottom;
		};
		Vector2 extremes[2];
		Vector2 r[2];
	};
};

struct alignas(16) _AxisAlignedBox : public AllocAABox {

	typedef AxisAlignedBox type;
	static _NexBaseAPI const AxisAlignedBox LargestBox;
	static _NexBaseAPI const AxisAlignedBox InvalidBox;

	union {
		struct {
			Vector3A minPoint;
			Vector3A maxPoint;
		};
		Vector3A extremes[2];
		Vector3A r[2];
	};
};

typedef _Rect::type Rect;

typedef Traits<_Vec2> TraitsVec2;
typedef Traits<_Vec3> TraitsVec3;
typedef Traits<_Vec3A> TraitsVec3A;
typedef Traits<_Vec4> TraitsVec4;
typedef Traits<_Plane> TraitsPlane;
typedef Traits<_Quat> TraitsQuat;
typedef Traits<_Matrix3x4> TraitsMat3x4;
typedef Traits<_Matrix4x4> TraitsMat4x4;
typedef TraitsMat4x4 TraitsMat4;
typedef Traits<_AxisAlignedBox> TraitsAABox;
typedef Traits<_Rect> TraitsRect;
typedef Traits<_AxisAngle> TraitsAxisAngle;
typedef Traits<_EulerAngles> TraitsEulerAngles;
typedef Traits<_PolarCoord> TraitsPolarCoord;
typedef Traits<IVector2> TraitsIVector2;
typedef Traits<IVector3> TraitsIVector3;
typedef Traits<IVector4> TraitsIVector4;

_NexTemplateExtern template class _NexBaseAPI PooledAllocator< _Matrix3x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE >;
_NexTemplateExtern template class _NexBaseAPI PooledAllocator< _Matrix4x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE >;
_NexTemplateExtern template class _NexBaseAPI PooledAllocator< Vector4, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE>;
_NexTemplateExtern template class _NexBaseAPI PooledAllocator<_AxisAlignedBox, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE>;

}
}

#include <math/VectorTraits.h>
#include <math/VectorMethods.h>
#include <math/VectorDefinitions.h>

#endif /* VECTORTYPES_H_ */
