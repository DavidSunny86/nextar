/*
 * Vector4A.h
 *
 *  Created on: Feb 10, 2014
 *      Author: obhi
 */

#ifndef VECTOR4A_H_
#define VECTOR4A_H_

#include "NexMath.h"
#include "MathTypes.h"

namespace nextar {
	/**
	 * @ Vector4A
	 **/
	/** @brief	Vector 4 set 0. **/
	inline Vector4A Vec4AZero();
	/** @brief	Vector 4 set x,y,z,w. **/
	inline Vector4A Vec4ASet(float x, float y, float z, float w);
	/** @brief	Vector 4 get x coordinate. **/
	inline float Vec4AGetX(Vec4AF q);
	/** @brief	Vector 4 get y coordinate. **/
	inline float Vec4AGetY(Vec4AF q);
	/** @brief	Vector 4 get z coordinate. **/
	inline float Vec4AGetZ(Vec4AF q);
	/** @brief	Vector 4 get w coordinate. **/
	inline float Vec4AGetW(Vec4AF q);
	/** @brief	Vector 4 dot x1*x2 + y1*y2 + z1*z2 + w1*w2. **/
	inline float Vec4ADot(Vec4AF vec1, Vec4AF vec2);
	/** @brief	Abs Vector4A **/
	inline Vector4A Vec4AAbs(Vec4AF vec3);
	/** @brief	Negate Vector4A **/
	inline Vector4A Vec4ANegate(Vec4AF vec3);
	/** @brief	Vector 4 Add. **/
	inline Vector4A Vec4AAdd(Vec4AF q1, Vec4AF q2);
	/** @brief	Vector 4 Sub. **/
	inline Vector3A Vec4ASub(Vec4AF q1, Vec4AF q2);
	/** @brief	Vector 4 Multiply and Add. **/
	inline Vector4A Vec4AMulAdd(Vec4AF q1, Vec4AF q2, Vec4AF q3);
	/** @brief	Vector 4 Replicate a value. **/
	inline Vector4A Vec4AReplicate(float value);
	/** @brief	Vector 4 Normalize (vec/sqrt(x*x + y*y + z*z + w*w)) **/
	inline Vector4A Vec4ANormalize(Vec4AF vec);
	/** @brief	Return true if any element in vec1 is greater than vec2 **/
	inline bool Vec4AGreaterAny(Vec4AF vec1, Vec4AF vec2);
	/** @brief	Return true if all elements in vec1 is greater than vec2 **/
	inline bool Vec4AGreaterAll(Vec4AF vec1, Vec4AF vec2);
	/** @brief	Return true if any element in vec1 is lesser than vec2 **/
	inline bool Vec4ALesserAny(Vec4AF vec1, Vec4AF vec2);
	/** @brief	Return true if all elements in vec1 is lesser than vec2 **/
	inline bool Vec4ALesserAll(Vec4AF vec1, Vec4AF vec2);
	/** @brief Interpolate */
	inline Vector4A Vec4ALerp(Vec4AF src, Vec4AF dst, float t);
	/** @brief	Vector 4 sqrt(x*x + y*y + z*z + w*w) **/
	inline float Vec4ALength(Vec4AF vec);
	/** @brief	Vector 4 (x*x + y*y + z*z + w*w) **/
	inline float Vec4ASqLength(Vec4AF vec);
	/** @brief	Vector 4  length(vec1-vec2)**/
	inline float Vec4ADistance(Vec4AF vec1, Vec4AF vec2);
	/** @brief	Vector 4  sqlength(vec1-vec2)**/
	inline float Vec4ASqDistance(Vec4AF vec1, Vec4AF vec2);

}

#endif /* VECTOR4A_H_ */
