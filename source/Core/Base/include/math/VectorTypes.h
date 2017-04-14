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


struct _Vec2 {
	typedef _Vec2 type; 
	union { 
		struct { 
			float x, y; 
		};
		float v[2]; 
	};
	static _NexBaseAPI const _Vec2 Origin;
};

struct _Vec3 { 
	typedef _Vec3 type; 
	union {
		struct {
			float x, y, z;
		};
		float v[3];
	};

	static _NexBaseAPI const _Vec3 XAxis;
	static _NexBaseAPI const _Vec3 YAxis;
	static _NexBaseAPI const _Vec3 ZAxis;
	static _NexBaseAPI const _Vec3 Origin;
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
typedef _Plane::type Plane3D;
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

typedef _IVecN<2> _IVector2;
typedef _IVecN<3> _IVector3;
typedef _IVecN<4> _IVector4;
typedef _IVector2 IVector2;
typedef _IVector3 IVector3;
typedef _IVector4 IVector4;

struct alignas(16) _Matrix3x4 : public AllocMatrix3x4 {
	typedef Matrix3x4 type;

	union {
		Vector3A r[3];
		float m[12];
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
	};
};

struct alignas(16) _Matrix4x4 : public AllocMatrix4x4 {
	typedef Matrix4x4 type;

	static _NexBaseAPI const Matrix4x4 IdentityMatrix;

	union {
		Vector3A r[4];
		float m[16];
		struct {
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
	};

	inline _Matrix4x4();
	inline _Matrix4x4(float m00, float m01, float m02, float m03, float m10,
		float m11, float m12, float m13, float m20, float m21, float m22,
		float m23, float m30, float m31, float m32, float m33);
	inline _Matrix4x4(std::initializer_list<float> l);
	inline _Matrix4x4(const Matrix3x4& m, const Vector3A& v);
	inline _Matrix4x4& operator =(const Matrix4x4& m);
};

struct _Rect {
	typedef _Rect type;

	union {
		Vector2 r[2];
		struct {
			Vector2 leftTop;
			Vector2 rightBottom;
		};
		struct {
			Vector2 min;
			Vector2 max;
		};
		Vector2 extremes[2];
	};
	inline _Rect();
	inline _Rect(float left, float top, float right, float bottom);
};

struct alignas(16) _AxisAlignedBox : public AllocAABox {

	typedef AxisAlignedBox type;
	static _NexBaseAPI const AxisAlignedBox LargestBox;
	static _NexBaseAPI const AxisAlignedBox InvalidBox;

	union {
		Vector3A r[2];
		float m[8];
		struct {
			Vector3A minPoint;
			Vector3A maxPoint;
		};
		Vector3A extremes[2];
	};

	inline _AxisAlignedBox();
	inline _AxisAlignedBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	inline _AxisAlignedBox(std::initializer_list<float> l);
};

typedef _Rect::type Rect2D;

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
typedef Traits<IVector2> TraitsIVec2;
typedef Traits<IVector3> TraitsIVec3;
typedef Traits<IVector4> TraitsIVec4;

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
