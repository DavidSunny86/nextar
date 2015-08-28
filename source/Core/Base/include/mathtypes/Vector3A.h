/*
 * Vector3A.h
 *
 *  Created on: Feb 9, 2014
 *      Author: obhi
 */

#ifndef VECTOR3A_H_
#define VECTOR3A_H_

#include <MathBase.h>
#include <MathTypes.h>

namespace nextar {

/** @brief	Vector 3 set 0. **/
inline Vector3A Vec3AZero();
/** @brief	Vector 3 set x,y,z,w. **/
inline Vector3A Vec3ASet(float x, float y, float z);
/** @brief	Vector 3 get x coordinate. **/
inline float Vec3AGetX(Vec3AF q);
/** @brief	Vector 3 get y coordinate. **/
inline float Vec3AGetY(Vec3AF q);
/** @brief	Vector 3 get z coordinate. **/
inline float Vec3AGetZ(Vec3AF q);
/** @brief	Vector 3 dot product. **/
inline float Vec3ADot(Vec3AF vec1, Vec3AF vec2);
/** @brief	Vector3A from Vector3. **/
inline Vector3A Vec3AFromVec3(const Vector3& vec3);
/** @brief	Vector3A from Vector4A. **/
inline Vector3A Vec3AFromVec4A(Vec4AF vec4);
/** @brief	Vector3A Add. **/
inline Vector3A Vec3AAdd(Vec3AF q1, Vec3AF q2);
/** @brief	Vector3A Sub. **/
inline Vector3A Vec3ASub(Vec3AF q1, Vec3AF q2);
/** @brief	Vector3A Mul. **/
inline Vector3A Vec3AMul(Vec3AF q1, Vec3AF q2);
/** @brief	Abs Vector4A **/
inline Vector3A Vec3AAbs(Vec3AF vec3);
/** @brief	Negate Vector3A **/
inline Vector3A Vec3ANegate(Vec3AF vec3);
/** @brief	Multiply scalar to Vector3A **/
inline Vector3A Vec3AMulScalar(Vec3AF v, float fac);
/** @brief	Result = Vector3A*Mat4x4. Same as  Mat4x4TransVec3 **/
inline Vector3A Vec3AMulMat4x4(Vec3AF v, Mat4x4F m2);
/** @brief	Vector 3 Multiply and Add q1*q2 + q3. **/
inline Vector3A Vec3AMulAdd(Vec3AF q1, Vec3AF q2, Vec3AF q3);
/** @brief	Vector 3 Normalize (vec/sqrt(x*x + y*y + z*z)) **/
inline Vector3A Vec3ANormalize(Vec3AF vec);
/** @brief	Return true if any element(x,y,z) in vec1 is greater than vec2 **/
inline bool Vec3AGreaterAny(Vec3AF vec1, Vec3AF vec2);
/** @brief	Return true if all elements(x,y,z) in vec1 is greater than vec2 **/
inline bool Vec3AGreaterAll(Vec3AF vec1, Vec3AF vec2);
/** @brief	Return true if any element(x,y,z) in vec1 is lesser than vec2 **/
inline bool Vec3ALesserAny(Vec3AF vec1, Vec3AF vec2);
/** @brief	Return true if all elements(x,y,z) in vec1 is lesser than vec2 **/
inline bool Vec3ALesserAll(Vec3AF vec1, Vec3AF vec2);
/** @brief	Return true if all elements(x,y,z) in vec1 is lesser than vec2 **/
inline Vector3A Vec3ACross(Vec3AF vec1, Vec3AF vec2);
/** @brief Interpolate */
inline Vector3A Vec3ALerp(Vec3AF src, Vec3AF dest, float t);
/** @brief	Vector 3 sqrt(x*x + y*y + z*z) **/
inline float Vec3ALength(Vec3AF vec);
/** @brief	Vector 3 (x*x + y*y + z*z) **/
inline float Vec3ASqLength(Vec3AF vec);
/** @brief	Vector 3  length(vec1-vec2)**/
inline float Vec3ADistance(Vec3AF vec1, Vec3AF vec2);
/** @brief	Vector 3  sqlength(vec1-vec2)**/
inline float Vec3ASqDistance(Vec3AF vec1, Vec3AF vec2);
/** @todo coordinate system from vector, the third should be v3 = cross(v1, v2) */
//inline Vector3A Vec3AGetOrtho(Vec3AF vec1);

}

#endif /* VECTOR3A_H_ */
