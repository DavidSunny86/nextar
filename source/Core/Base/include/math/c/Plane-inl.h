/* 
 * File:   _plane.h
 * Author: obhi
 *
 * Created on January 24, 2011, 8:12 PM
 */

#ifndef NEXTAR_CORE_PLANE_H
#define	NEXTAR_CORE_PLANE_H

namespace nextar {

inline float PlaneDotVec3A(PlaneF p, Vec3AF v) {
	return p.x * v.x + p.y * v.y + p.z * v.z + p.w;
}

inline Vector3A PlaneAbsNormal(PlaneF p) {
	return QuadInit(Math::Abs(p.x), Math::Abs(p.y), Math::Abs(p.z));
}

}
#endif	/* NEXTAR_CORE_PLANE_H */

