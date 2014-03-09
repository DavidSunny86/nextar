#include <BaseHeaders.h>
#include <BoundingVolume.h>
#include <CommonPools.h>

namespace nextar {

	/*********************************
	 * BoundingBox 
	 *********************************/
	BoundingBox::BoundingBox() {
		// largest bv
		center = origCenter = Vec3AZero();
		extends = origExtends = Vec3AZero();
	}

	void BoundingBox::UpdateBounds(Mat4x4F m) {
		center = Mat4x4TransVec3A(origCenter, m);
		extends = Mat4x4TransBoundRadius(origExtends, m);
	}

	void BoundingBox::UpdateBounds(float scale, QuatF rot, Vec3AF pos) {
		center = Vec3AAdd(QuatTransVec3A(rot, origCenter), pos);
		extends = QuatTransBoundRadius(Vec3AMulScalar(origExtends, scale), rot);
	}

	void BoundingBox::SetNull() {
		center = origCenter = extends = origExtends = Vec3AZero();
	}

	void BoundingBox::SetVolume(Vec3AF center, Vec3AF extends) {
		this->center = this->origCenter = center;
		this->extends = this->origExtends = extends;
	}

	void BoundingBox::UpdateBounds(const Vector3A* pt, uint32 numPts) {
		if (numPts) {
			/* Costly calculate to encompass all points */

			/*
			 u32 minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
			 for (u32 i = 1; i < numPts; i++) {
			 if (pt[i].x < pt[minx].x) minx = i;
			 if (pt[i].x > pt[maxx].x) maxx = i;
			 if (pt[i].y < pt[miny].y) miny = i;
			 if (pt[i].y > pt[maxy].y) maxy = i;
			 if (pt[i].z < pt[minz].z) minz = i;
			 if (pt[i].z > pt[maxz].z) maxz = i;
			 }

			 // Compute the squared distances for the three pairs of points
			 float dist2x = Vec3ADot(Vec3ASub(pt[maxx],pt[minx]),
			 Vec3ASub(pt[maxx],pt[minx]));
			 float dist2y = Vec3ADot(Vec3ASub(pt[maxy],pt[miny]),
			 Vec3ASub(pt[maxy],pt[miny]));
			 float dist2z = Vec3ADot(Vec3ASub(pt[maxz],pt[minz]),
			 Vec3ASub(pt[maxz],pt[minz]));
			 // Pick the pair (min,max) of points most distant
			 u32 minPt = minx;
			 u32 maxPt = maxx;
			 if (dist2y > dist2x && dist2y > dist2z) {
			 maxPt = maxy;
			 minPt = miny;
			 }
			 if (dist2z > dist2x && dist2z > dist2y) {
			 maxPt = maxz;
			 minPt = minz;
			 }

			 center = origCenter = */
			NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
		}
	}

	/*********************************
	 * BoundingVolume
	 *********************************/
	BoundingVolume::BoundingVolume() {
		box = NEX_NEW BoundingBox;
		radius = 0;
	}

	BoundingVolume::~BoundingVolume() {
		NEX_DELETE (box);
	}
		
	void BoundingVolume::MergeBounds(const BoundingVolume& vol) {
		Vector3A a = Vec3AAbs(Vec3ASub(box->center, vol.box->center));
		Vector3A b = Vec3AAdd(box->center, vol.box->center);
		box->center = box->origCenter = Vec3AMulScalar(b, 0.5f);
		box->extends = box->origExtends = Vec3AAdd(a, Vec3AAdd(vol.box->extends, box->extends));
		radius += (vol.radius + Vec3ADot(a, a));
	}

	void BoundingVolume::SetVolume(Vec3AF center, Vec3AF extends, float radius) {
		box->SetVolume(center, extends);
		this->radius = radius;
	}

}
