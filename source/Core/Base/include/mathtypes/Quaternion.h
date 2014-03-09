/*
 * Quaternion.h
 *
 *  Created on: Feb 10, 2014
 *      Author: obhi
 */

#ifndef QUATERNION_H_
#define QUATERNION_H_


#include <MathBase.h>
#include <MathTypes.h>

namespace nextar {
	/**
	 * @ Quaternion
	 **/
	/** @brief Quaternion set **/
	inline Quaternion QuatSet(float x, float y, float z, float w);
	/** @brief Quaternion get identity quat. **/
	inline Quaternion QuatIdentity();
	/** @brief Quaternion get x coordinate. **/
	inline float QuatGetX(QuatF q);
	/** @brief Quaternion get y coordinate. **/
	inline float QuatGetY(QuatF q);
	/** @brief Quaternion get z coordinate. **/
	inline float QuatGetZ(QuatF q);
	/** @brief Quaternion get w coordinate. **/
	inline float QuatGetW(QuatF q);
	/** @brief Quaternion from axis angle. **/
	inline Quaternion QuatFromAxisAng(const Vector3& axis, float ang);
	/** @brief Quaternion from axis angle. **/
	inline Quaternion QuatFromAxisAng(Vec3AF axis, float ang);
	/** @brief Quaternion from rotation matrix */
	inline Quaternion QuatFromMat4x4(Mat4x4F m);
	/** @brief Quaternion from rotation matrix */
	inline Quaternion QuatFromMat3x4(Mat3x4F m);
	/** Multiply two quaternions */
	inline Quaternion QuatMul(QuatF q1, QuatF q2);
	/** Rotate/Transform a vector using a quaternion */
	inline Vector3A QuatTransVec3A(QuatF q, Vec3AF v);
	/** Transform bound radius by quaternion */
	inline Vector3A QuatTransBoundRadius(QuatF q, Vec3AF r);
	/** Perform quaternion slerp */
	inline Quaternion QuatSlerp(QuatF qStart, QuatF qEnd, float amount);
	/** Perform quaternion lerp */
	inline Quaternion QuatLerp(QuatF from, QuatF to, float t);
	/** Quaternion normalize */
	inline Quaternion QuatNormalize(QuatF vec);
	/** Quaternion dot */
	inline float QuatDot(QuatF q1, QuatF q2);
	/** @brief	Return true if any element in vec1 is greater than vec2 **/
	inline bool QuatGreaterAny(QuatF q1, QuatF q2);
	/** @brief	Return true if all elements in q1 is greater than q2 **/
	inline bool QuatGreaterAll(QuatF q1, QuatF q2);
	/** @brief	Return true if any element in q1 is lesser than q2 **/
	inline bool QuatLesserAny(QuatF q1, QuatF q2);
	/** @brief	Return true if all elements in q1 is lesser than q2 **/
	inline bool QuatLesserAll(QuatF q1, QuatF q2);
	/** @brief Returns the inverse of the quaternion, assuming the
	 * quaternion has been normalized */
	inline Quaternion QuatInverse(QuatF q);

}


#endif /* QUATERNION_H_ */
