/*
 * VectorTypes.h
 *
 *  Created on: Feb 9, 2014
 *      Author: obhi
 */

#ifndef VECTORTYPES_H_
#define VECTORTYPES_H_

#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE
#	include "mathtypes/sse/SSE_VectorTypes.h"
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_FALLBACK
#	include "mathtypes/c/C_VectorTypes.h"
#else
#endif
#include <NexBase.h>
#include <CommonPools.h>
namespace nextar {

struct _Matrix3x4;
struct _Matrix4x4;

#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE

#ifdef _MSC_VER
typedef __declspec(align(16)) _Matrix3x4 Matrix3x4;
typedef __declspec(align(16)) _Matrix4x4 Matrix4x4;
#else
using Matrix3x4 alignas(16) = _Matrix3x4;
using Matrix4x4 alignas(16) = _Matrix4x4;
#endif

#else
using Matrix3x4 = _Matrix3x4;
using Matrix4x4 = _Matrix4x4;
#endif

typedef AllocMathPool< _Matrix3x4, NEX_MATRIX_POOL_NUM_PER_BLOCK > AllocMatrix3x4;
typedef AllocMathPool< _Matrix4x4, NEX_MATRIX_POOL_NUM_PER_BLOCK > AllocMatrix4x4;
typedef PooledAllocator< _Matrix3x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE > AllocatorMatrix3x4;
typedef PooledAllocator< _Matrix4x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE > AllocatorMatrix4x4;
typedef PooledAllocator<Vector3A, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE> AllocatorVector3A;
typedef PooledAllocator<Vector4A, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE> AllocatorVector4A;
typedef PooledAllocator<Quaternion, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE> AllocatorQuaternion;

struct _NexBaseAPI _Matrix3x4 : public AllocMatrix3x4 {

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
	inline _Matrix3x4() {
	}

	inline const float operator [](size_t i) const {
		return m[i];
	}

	inline float& operator [](size_t i) {
		return m[i];
	}

	inline float& operator ()(int i, int j) {
		return m[i * 4 + j];
	}

	inline float operator ()(int i, int j) const {
		return m[i * 4 + j];
	}

	inline _Matrix3x4& operator =(const Matrix3x4&);
};

struct _NexBaseAPI _Matrix4x4: public AllocMatrix4x4 {
	static const Matrix4x4 IdentityMatrix;

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
	inline _Matrix4x4() {
	}

	inline _Matrix4x4(const Matrix3x4&, const Vector4A&);

	inline _Matrix4x4(float m00, float m01, float m02, float m03, float m10,
			float m11, float m12, float m13, float m20, float m21, float m22,
			float m23, float m30, float m31, float m32, float m33);

	inline const float operator [](size_t i) const {
		return m[i];
	}

	inline float& operator [](size_t i) {
		return m[i];
	}

	inline float& operator ()(int i, int j) {
		return m[i * 4 + j];
	}

	inline float operator ()(int i, int j) const {
		return m[i * 4 + j];
	}

	inline Matrix4x4 & operator =(const Matrix4x4&);
};

typedef const Matrix3x4& Mat3x4F;
typedef const Matrix3x4& Mat3x4R;

typedef const Matrix4x4& Mat4x4F;
typedef const Matrix4x4& Mat4x4R;

}

#include <mathtypes/Vector3A.h>
#include <mathtypes/Vector4A.h>
#include <mathtypes/Quaternion.h>
#include <mathtypes/Plane.h>
#include <mathtypes/Matrix3x4.h>
#include <mathtypes/Matrix4x4.h>

namespace nextar {


_NexTemplateExtern template class _NexBaseAPI PooledAllocator< _Matrix3x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE >;
_NexTemplateExtern template class _NexBaseAPI PooledAllocator< _Matrix4x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE >;
_NexTemplateExtern template class _NexBaseAPI PooledAllocator<Vector3A, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE>;
//_NexTemplateExtern template class _NexBaseAPI PooledAllocator<Vector4A, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE>;
//_NexTemplateExtern template class _NexBaseAPI PooledAllocator<Quaternion, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE>;


}
#endif /* VECTORTYPES_H_ */
