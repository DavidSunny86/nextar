//-------------------------
// Created on july 24 2009
//-------------------------
#ifndef _MATHTYPES_H_
#define _MATHTYPES_H_

#include <MathBase.h>
#include <NexAlloc.h>
#include <MemUtils.h>
#include <Randomizer.h>

// Specialize allocator
namespace nextar {

#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE
template<> class AllocatorBase<MEMCAT_MATH_CORE> : public AllocatorSimdAligned {
};
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_3DNOW
#error Define allocator
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_VMX128
template <> class AllocatorBase<MEMCAT_MATH_CORE> : public AllocatorSimdAligned {
};
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_FALLBACK
template <> class AllocatorBase<MEMCAT_MATH_CORE> : public AllocatorGeneral {
};
#else
#error Define allocator
#endif

template<typename T, const size_t NumPerBlock>
class AllocMathPool: public AllocPooledObjectBase<
		PooledAllocator<T, NumPerBlock, MEMCAT_MATH_CORE> > {
public:
	typedef PooledAllocator<T, NumPerBlock, MEMCAT_MATH_CORE> Allocator;
};


/**
 * @remarks Non vectorized members. These can be used as members to classes
 *          without explicit allocation as opposed to the case of Core types.
 **/
class Vector2;
class Vector3;
class Vector4;
class Matrix3x3;
class PolarSys;
class AABox3;
class AxisAngle;
class EularAngles;
class Box2D;

}

/* depends on platform and implementation */
#include <mathtypes/VectorTypes.h>
/**
 * Simple types, definitions separated to avoid inter-dependecy
 */
namespace nextar {
#include <mathtypes/Vector3.h>
#include <mathtypes/Vector2.h>
#include <mathtypes/Vector4.h>
#include <mathtypes/Matrix3x3.h>
#include <mathtypes/Box2D.h>
#include <mathtypes/AABox3.h>
#include <mathtypes/AxisAngle.h>
#include <mathtypes/EularAngles.h>
#include <mathtypes/PolarSys.h>
#include <mathtypes/Vector3-inl.h>
#include <mathtypes/Matrix3x3-inl.h>

}

#include <mathtypes/VectorTypes-inl.h>

namespace nextar {
#ifdef NEX_MATH_RANDOMIZER_DEFINED

/** Specialize sse randomizer **/
template<> class RandomizerBase<RANDCLASS_CORE> {
public:

	enum {
		MAX_VAL = 0xffffffff
	};

	static inline void Seed(uint32 s) {
		MathRandomizer::Seed(s);
	}

	static inline uint32 Rand() {
		unsigned int ret;
		MathRandomizer::RandSSE(&ret);
		return ret;
	}
};

#endif

namespace Math {

	template <typename T>
	class Traits {
	public:
		inline static uint32 ElementCount() {
			return 1;
		}

		inline static bool Equals(const T& v1, const T& v2) {
			return v1 - v2 == 0;
		}

		inline static uint32 Hash(const T& v) {
			return (uint32)v;
		}

		inline static uint32 Size() {
			return sizeof(T);
		}
	};

	template <>
	class Traits<float> {
	public:
		static uint32 ElementCount() {
			return 1;
		}

		inline static bool Equals(const float v1, const float v2) {
			if (Math::AlmostEqualUlps(v1, v2, 2))
				return true;
			return Math::AlmostEqualRelativeOrAbsolute(v1, v1, MAX_RELATIVE_ERROR,
					std::numeric_limits<float>::min());
		}

		inline static uint32 Hash(const float v) {
			return (uint32)(v);
		}

		inline static uint32 Size() {
			return sizeof(float);
		}
	};

	template <>
	class Traits<Vector2> {
	public:
		inline static uint32 ElementCount() {
			return 2;
		}

		inline static bool Equals(const Vector2& v1, const Vector2& v2) {
			return (bool)( Math::Traits<float>::Equals(v1.x,v2.x) &&
					Math::Traits<float>::Equals(v1.y,v2.y) );
		}

		inline static uint32 Hash(const Vector2& v) {
			return (uint32)((v.x+v.y));
		}

		inline static uint32 Size() {
			return sizeof(float) * 2;
		}
	};

	template <>
	class Traits<Vector3> {
	public:
		inline static uint32 ElementCount() {
			return 3;
		}

		inline static bool Equals(const Vector3& v1, const Vector3& v2) {
			return (bool)( Math::Traits<float>::Equals(v1.x,v2.x) &&
						Math::Traits<float>::Equals(v1.y,v2.y) &&
						Math::Traits<float>::Equals(v1.z,v2.z));
		}

		inline static uint32 Hash(const Vector3& v) {
			return (uint32)((v.x+v.y+v.z));
		}

		inline static uint32 Size() {
			return sizeof(float) * 3;
		}
	};

	template <>
	class Traits<Vector4> {
	public:
		inline static uint32 ElementCount() {
			return 4;
		}

		inline static bool Equals(const Vector4& v1, const Vector4& v2) {
			return (bool)( Math::Traits<float>::Equals(v1.x,v2.x) &&
						Math::Traits<float>::Equals(v1.y,v2.y) &&
						Math::Traits<float>::Equals(v1.z,v2.z) &&
						Math::Traits<float>::Equals(v1.w,v2.w));
		}

		inline static uint32 Hash(const Vector4& v) {
			return (uint32)((v.x+v.y+v.z+v.w));
		}

		inline static uint32 Size() {
			return sizeof(float) * 4;
		}
	};
}

typedef std::array<int32, 2> IVector2;
typedef std::array<int32, 3> IVector3;
typedef std::array<int32, 4> IVector4;



}

// naming convention for operations
//  * short names:
//  $ data types
//   float         :  Scalar
//   Quad          :  Quad
//   Vector2       :  Vec2
//   Vector3       :  Vec3
//   Vector3A      :  Vec3A
//   Vector4A      :  Vec4A
//   Matrix3x3     :  Mat3x3
//   Matrix3x4     :  Mat3x4
//   Matrix4x4     :  Mat4x4
//   PolarSys      :  Polar
//   Quaternion    :  Quat
//   Plane         :  Plane
//   QuatPosSys    :  QuatPosSys
//   Transform3x4  :  Trans3x4
//   AxisAngle     :  AxisAng
//   EularAngles   :  Eular
//   AABox2        :  AABox2
//   AABox3        :  AABox3
//  $ operations
//   Multiplication                              :  Mul
//   Division                                    :  Div
//   Addition                                    :  Add
//   Substraction                                :  Sub
//   Dot product                                 :  Dot
//   Cross product                               :  Cross
//   Distance                                    :  Distance
//   Copy                                        :  Cpy
//   Square                                      :  Square
//   Square Root                                 :  Sqrt
//   Transform (Space)			                  :  Trans
//   Exponent                                    :  Exp
//   Natural Log                                 :  Ln
//   Log Base 10                                 :  Log
//   Linear Interpolation                        :  Lerp
//   Spherical Linear Interpolation              :  Slerp
//   Spline Interpolation                        :  Spline
//   Inplace Invert                              :  Invert
//   Return Inverted                             :  Inverse
//   Inplace Conjugate                           :  Conjugate
//   Inplace Normalize                           :  Normalize
//  conventions
//  * type conversion rule: use From notion i.e. Vector3::FromVec4A
//  * common operations like Dot, always have same name for C++
//  * operations that any object performs on any other object always
//    define that operation. eg, Matrix3x3::RotateVec3 or Mat3x3RotateVec3
//  * PTR_PRESERVED indicates that the store parameter can be any of the input
//    parameters.
//  * for binary operations like multiplication,addition,etc. if the left hand
//    operand is of same type as right hand then the function name should only
//    mention the left hand operand eg. for Vector3&Vector3 the C function
//    would be name Vec3Mul not Vec3MulVec3

/** @remarks Core elements as member variables can be easily set up using this macro and
 *           CoreVars template
 **/

#endif //_MATHTYPES_H_
