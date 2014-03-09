/*
 * SSE_VectorTypes.h
 *
 *  Created on: Feb 9, 2014
 *      Author: obhi
 */

#ifndef SSE_VECTORTYPES_H_
#define SSE_VECTORTYPES_H_

#include <MathBase.h>

namespace nextar {
	typedef __m128 Quad;
	typedef const Quad QuadPF; // fast param
	typedef const Quad& QuadPR; // ref param
	typedef Quad Vector3A;
	typedef Quad Vector4A;
	typedef Quad Quaternion;
	typedef Quad Plane;

	typedef QuadPF Vec3AF;
	typedef QuadPR Vec3AR;

	typedef QuadPF Vec4AF;
	typedef QuadPR Vec4AR;

	typedef QuadPF QuatF;
	typedef QuadPR QuatR;

	typedef QuadPF PlaneF;
	typedef QuadPR PlaneR;

	/**
	 @remarks Quad related functions
	 **/

	/** @remarks Load unaligned floats **/
	inline Quad QuadLoad3UF(const float* vals);
	inline Quad QuadLoad4UF(const float* vals);

	/** @remarks Load aligned floats **/
	inline Quad QuadLoad3AF(const float* vals);
	inline Quad QuadLoad4AF(const float* vals);

	/** @remarks Set elements **/
	inline Quad QuadSet(float x, float y, float z, float w);
	inline Quad QuadZero();
	inline Quad QuadSetX(QuadPF q, float val);
	inline Quad QuadSetY(QuadPF q, float val);
	inline Quad QuadSetZ(QuadPF q, float val);

	/** @remarks Get elements **/
	inline float QuadGetByIdx(QuadPF q, size_t idx);
	inline float QuadGetX(QuadPF q);
	inline float QuadGetY(QuadPF q);
	inline float QuadGetZ(QuadPF q);
	inline float QuadGetW(QuadPF q);

	/** @remarks Replicat X component all over **/
	inline Quad QuadSplatX(QuadPF q);
	/** @remarks Replicat Y component all over **/
	inline Quad QuadSplatY(QuadPF q);
	/** @remarks Replicat Z component all over **/
	inline Quad QuadSplatZ(QuadPF q);
	/** @remarks Replicat W component all over **/
	inline Quad QuadSplatW(QuadPF q);
	/** @remarks Replicat val all over **/
	inline Quad QuadReplicate(float val);

	/** @remarks Abs elements **/
	inline Quad QuadAbs(QuadPF q);
	/** @remarks Negate elements **/
	inline Quad QuadNegate(QuadPF q);
	/** @remarks Negate elements **/
	inline Quad QuadAdd(QuadPF a, QuadPF b);
	/** @remarks Negate elements **/
	inline Quad QuadSub(QuadPF a, QuadPF b);
	/** @remarks Negate elements **/
	inline Quad QuadMul(QuadPF a, QuadPF b);
	/** @remarks Negate elements **/
	inline Quad QuadDiv(QuadPF a, QuadPF b);
	/** @remarks Scalar multiply **/
	inline Quad QuadMulScalar(QuadPF q, float val);
	/** @remarks Multiply and Add: a*b+c **/
	inline Quad QuadMulAdd(QuadPF a, QuadPF b, QuadPF c);
	/** @remarks Splat Sin and Cos value of 'val' **/
	inline void QuadSinCos(Quad& vsinval, Quad& vcosval, const float val);
	/** @reamarks Calculates reciprocal square root */
	inline Quad QuadInvSqrt(QuadPF);
	/* Quad select */
	inline Quad QuadSelect(QuadPF q1, QuadPF q2);
}

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
#define Vec4AZero       QuadZero
#define Vec4ASet        QuadSet
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

#endif /* SSE_VECTORTYPES_H_ */
