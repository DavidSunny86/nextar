#include <BaseHeaders.h>
#include <Intersect.h>

namespace nextar {
namespace Intersect {

_NexBaseAPI IntersectionResult BoundingVolumes(const BoundingVolume& vol1,
		const BoundingVolume& vol2) {

	Vec3A::type d = Vec3ASub(vol1.GetCenter(), vol2.GetCenter());
	float dist = Vec3ADot(d, d);
	float rad = vol1.GetRadius() + vol2.GetRadius();
	if (dist > rad * rad)
		return IR_OUTSIDE;
	else
		return (Vec3AGreaterAny(Vec3AAbs(d),
		Vec3AAdd(vol1.GetExtends(), vol2.GetExtends()))) ?
				IR_OUTSIDE : IR_OUTSIDE;
}

_NexBaseAPI IntersectionResult BoundingVolumeFrustumCoherent(
		const BoundingVolume& vol, const Frustum& frustum, uint32 inMask,
		uint32& outMask, uint32& lastPlane) {

	uint32 i, k = 1 << lastPlane;
	IntersectionResult result = IR_INSIDE;
	const Plane::type *planes = frustum.GetPlanes();
	outMask = 0;
	if (k & inMask) {
		float m = PlaneDotVec3A(planes[lastPlane], vol.GetExtends());
		Vec3A::type absNorm = PlaneAbsNormal(planes[lastPlane]);
		float n = Vec3ADot(absNorm, vol.GetExtends());

		if (m + n < 0)
			return IR_OUTSIDE;
		if (m - n < 0) {
			outMask |= k;
			result = IR_INTERSECTS;
		}
	}

	uint32 numPlanes = (uint32) frustum.GetNumPlanes();
	for (i = 0, k = 1; i < numPlanes; i++, k += k) {

		if ((i != lastPlane) && (k & inMask)) {
			float m = PlaneDotVec3A(planes[i], vol.GetExtends());
			Vec3A::type absNorm = PlaneAbsNormal(planes[i]);
			float n = Vec3ADot(absNorm, vol.GetExtends());

			if (m + n < 0) {
				lastPlane = i;
				return IR_OUTSIDE;
			}
			if (m - n < 0) {
				outMask |= k;
				result = IR_INTERSECTS;
			}
		}
	}
	return result;
}

_NexBaseAPI IntersectionResult BoundingVolumeFrustum(const BoundingVolume& vol,
		const Frustum& frustum) {
	IntersectionResult result = IR_INSIDE;
	size_t numPlanes = frustum.GetNumPlanes();
	const Plane::type* planes = frustum.GetPlanes();
	for (size_t i = 0; i < numPlanes; ++i) {
		float m = PlaneDotVec3A(planes[i], vol.GetExtends());
		Vec3A::type absNorm = PlaneAbsNormal(planes[i]);
		float n = Vec3ADot(absNorm, vol.GetExtends());

		if (m + n < 0)
			return IR_OUTSIDE;
		if (m - n < 0)
			result = IR_INTERSECTS;
	}
	return result;
}

_NexBaseAPI IntersectionResult BoundingSphereFrustum(Vec3A::pref center,
		float radius, const Frustum& frustum) {
	size_t numPlanes = frustum.GetNumPlanes();
	const Plane::type* planes = frustum.GetPlanes();
	for (size_t i = 0; i < numPlanes; ++i) {
		float c = PlaneDotVec3A(planes[i], center);
		if (c > radius)
			return IR_OUTSIDE;
		if (c > -radius)
			return IR_INTERSECTS;
	}
	return IR_INSIDE;
}
}
}
