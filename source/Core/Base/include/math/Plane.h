/*
 * Plane.h
 *
 *  Created on: Feb 10, 2014
 *      Author: obhi
 */

#ifndef PLANE_H_
#define PLANE_H_

#include <MathBase.h>
#include <MathTypes.h>

namespace nextar {
/**
 * @ Plane
 **/
/** @brief	Vector 4 Normalize (vec/sqrt(x*x + y*y + z*z)) **/
inline Plane PlaneNormalize(PlaneF vec);
/** @brief	Dot operation (plane.normal dot vec) **/
inline float PlaneNormalDotVec3A(PlaneF p, Vec3AF vec);
/** @brief	Dot operation (plane.normal dot vec) + d **/
inline float PlaneDotVec3A(PlaneF p, Vec3AF vec);
/** @brief	Returns abs(plane.normal) **/
inline Vector3A PlaneAbsNormal(PlaneF p);

}

#endif /* PLANE_H_ */
