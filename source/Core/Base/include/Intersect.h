/* 
 * File:   Intersect.h
 * Author: obhi
 *
 * Created on January 29, 2011, 11:05 AM
 */

#ifndef NEXTAR_INTERSECTIONS_H
#define	NEXTAR_INTERSECTIONS_H

#include <BoundingVolume.h>

namespace nextar {

	namespace Intersect {

		enum IntersectionResult {
			IR_OUTSIDE = 0, IR_INSIDE, IR_INTERSECTS
		};

		/**
		 * @remarks Test bounding volume frustum intersection using coherency
		 *          and masking.
		 */
		_NexBaseExport IntersectionResult BoundingVolumeFrustumCoherent(
				const BoundingVolume& vol, const Frustum& frustum, uint32 inMask,
				uint32& newMask, uint32& lastPlane);

		/** @remarks Test bounding volume frustum intersection */
		_NexBaseExport IntersectionResult BoundingVolumeFrustum(
				const BoundingVolume& vol, const Frustum& frustum);

		/** @remarks Test bounding volume bounding volume */
		_NexBaseExport IntersectionResult BoundingVolumes(
				const BoundingVolume& vol1, const BoundingVolume& vol2);

		/** @remarks Intersect sphere with frustum */
		_NexBaseExport IntersectionResult BoundingSphereFrustum(Vec3AF center,
				float radius, const Frustum& frustum);
	}
}

#endif	/* NEXTAR_INTERSECTIONS_H */

