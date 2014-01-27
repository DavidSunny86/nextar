/* 
 * File:   boundingVolume.h
 * Author: obhi
 *
 * Created on January 27, 2011, 4:58 AM
 */

#ifndef BOUNDING_VOLUME_H
#define	BOUNDING_VOLUME_H

#include "NexMath.h"
#include "Frustum.h"

namespace nextar {

	struct BoundsInfo {
		Vector3 origCenter;
		Vector3 origRadius;
		float linearRadius;
	};

	class _NexExport BoundingVolume: public AllocMathCore {
	public:
		static BoundingVolume Null;

		Vector3A center;
		/* half extends dx/2, dy/2, dz/2*/
		Vector3A radius;
		/* mostly 0,0,0 */
		Vector3A origCenter;
		Vector3A origRadius;

		float linearRadius;
		

		BoundingVolume();
		BoundingVolume(Vec3AF center, Vec3AF extends, float radius);

		void SetNull();
		void SetVolume(Vec3AF center, Vec3AF extends, float radius);
		void MergeBounds(const BoundingVolume&);
		void UpdateBounds(Mat4x4F m);
		void UpdateBounds(float scale, QuatF rot, Vec3AF pos);
		void UpdateBounds(const Vector3A* points, uint32 maxPoints);

	};
}

#endif	/* BOUNDING_VOLUME_H */

