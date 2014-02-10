//-------------------------
//@ Created on july 24 2009
//-------------------------
#ifndef NEXTAR_MATHTYPES_H
#define NEXTAR_MATHTYPES_H

#ifndef NEXTAR_MATH
#error Do not include this file directly, Include  NexMath.h
#endif

#include "NexAlloc.h"
#include "MemUtils.h"
#include "Randomizer.h"

//@ Macros

namespace nextar {
	namespace Math {

	}
}

#ifdef NEX_MSVC
#ifdef NEX_VECTOR_HARDWARE_USED
#	define NEXCORE_FASTCALL __fastcall
#endif
#endif
#ifndef NEXCORE_FASTCALL
#	define NEXCORE_FASTCALL
#endif

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

	/**
	 * @remarks Core types are different from the Generic Math types in that
	 *          they are not managed, and procedural in apporach. Core types
	 *          cannot be allocated by new and delete. They dont support these
	 *          operations. The allocator supported by them need to be used
	 *          explicitly. Also parametrs to function uses different conventions.
	 *
	 **/
	using namespace ::nextar;

#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE
#include "mathtypes/sse/NexSSETypes.h"
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_3DNOW
#include "mathtypes/3dnow/Nex3dNowTypes.h"
#error Not implemented for this platform
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_VMX128
#include "mathtypes/vmx128/NexVMXTypes.h"
#error Not implemented for this platform
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_FALLBACK
#include "mathtypes/c/NexGenericTypes.h"
#else
#error Not implemented for this platform
#endif

	/** List of core functions. All are inlined, and all architecture needs to
	 *  define these set of functions.
	 **/
#include "mathtypes/CoreFunctionsDecl.h"

	/**
	 * Simple types, definitions separated to avoid inter-dependecy
	 */
#include "mathtypes/Vector3.h"
#include "mathtypes/Vector2.h"
#include "mathtypes/Vector4.h"
#include "mathtypes/Matrix3x3.h"
#include "mathtypes/Box2D.h"
#include "mathtypes/AABox3.h"
#include "mathtypes/AxisAngle.h"
#include "mathtypes/EularAngles.h"
#include "mathtypes/PolarSys.h"

#include "mathtypes/Vector3-inl.h"
#include "mathtypes/Matrix3x3-inl.h"

	/** Core implementations. **/
#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE
#include <mathtypes/sse/NexSSETypes-inl.h>
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_3DNOW
#include <mathtypes/sse/nexcore_3dnowtypes-inl.h>
#error Not implemented for this platform
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_VMX128
#include <mathtypes/vmx128/nexcore_vmxtypes-inl.h>
#error Not implemented for this platform
#elif NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_FALLBACK
#include <mathtypes/c/NexGenericTypes-inl.h>
#else
#error Not implemented for this platform
#endif

//@ naming convention for operations
//#  * short names:

//#  $ data types
//#   float         :  Scalar
//#   Quad          :  Quad
//#   Vector2       :  Vec2
//#   Vector3       :  Vec3
//#   Vector3A      :  Vec3A
//#   Vector4A      :  Vec4A
//#   Matrix3x3     :  Mat3x3
//#   Matrix3x4     :  Mat3x4
//#   Matrix4x4     :  Mat4x4
//#   PolarSys      :  Polar
//#   Quaternion    :  Quat
//#   Plane         :  Plane
//#   QuatPosSys    :  QuatPosSys
//#   Transform3x4  :  Trans3x4
//#   AxisAngle     :  AxisAng
//#   EularAngles   :  Eular
//#   AABox2        :  AABox2
//#   AABox3        :  AABox3

//#  $ operations
//#   Multiplication                              :  Mul
//#   Division                                    :  Div
//#   Addition                                    :  Add
//#   Substraction                                :  Sub
//#   Dot product                                 :  Dot
//#   Cross product                               :  Cross
//#   Distance                                    :  Distance
//#   Copy                                        :  Cpy
//#   Square                                      :  Square
//#   Square Root                                 :  Sqrt
//#   Transform (Space)			                  :  Trans
//#   Exponent                                    :  Exp
//#   Natural Log                                 :  Ln
//#   Log Base 10                                 :  Log
//#   Linear Interpolation                        :  Lerp
//#   Spherical Linear Interpolation              :  Slerp
//#   Spline Interpolation                        :  Spline
//#   Inplace Invert                              :  Invert
//#   Return Inverted                             :  Inverse
//#   Inplace Conjugate                           :  Conjugate
//#   Inplace Normalize                           :  Normalize

//@  conventions
//#  * type conversion rule: use From notion i.e. Vector3::FromVec4A
//#  * common operations like Dot, always have same name for C++
//#  * operations that any object performs on any other object always
//#    define that operation. eg, Matrix3x3::RotateVec3 or Mat3x3RotateVec3
//#  * PTR_PRESERVED indicates that the store parameter can be any of the input
//#    parameters.
//#  * for binary operations like multiplication,addition,etc. if the left hand
//#    operand is of same type as right hand then the function name should only
//#    mention the left hand operand eg. for Vector3&Vector3 the C function
//#    would be name Vec3Mul not Vec3MulVec3

	/** @remarks Core elements as member variables can be easily set up using this macro and
	 *           CoreVars template
	 **/

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
		;
	};

#endif
} // namespace nextar

#endif //NEXTAR_MATHTYPES_H
