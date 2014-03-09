/*
 * Matrix3x4.h
 *
 *  Created on: Feb 10, 2014
 *      Author: obhi
 */

#ifndef MATRIX3X4_H_
#define MATRIX3X4_H_


#include <MathBase.h>
#include <MathTypes.h>

namespace nextar {
#ifndef Mat3x4Row
#	define Mat3x4Row(m, i) (m).r[i]
#endif

	/**
	 * @ Matrix3x4
	 **/
	/** @brief	Matrix3x4 from vector mapping creates a matrix that
	 can rotate the vector axis1 to axis2 when post multiplied to axis1. **/
	inline Matrix3x4 Mat3x4FromVectorMapping(const Vector3& axis1,
			const Vector3& axis2);
	/** @brief Matrix3x4 transpose **/
	inline Matrix3x4 Mat3x4Transpose(Mat3x4F m);
	/** @brief rotate vector **/
	inline Vec3AF Mat3x4TransVec3A(Vec3AF v, Mat3x4F m);
	/** @brief Returns a look at matrix based on a look at vector and up position **/
	inline Matrix3x4 Mat3x4FromViewUp(Vec3AF viewDir, Vec3AF upDir);

}


#endif /* MATRIX3X4_H_ */
