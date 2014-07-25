/* 
 * File:   boundingVolume.h
 * Author: obhi
 *
 * Created on January 27, 2011, 4:58 AM
 */

#ifndef BOUNDING_VOLUME_H
#define	BOUNDING_VOLUME_H

#include <NexMath.h>
#include <Frustum.h>

namespace nextar {

struct BoundsInfo {
	Vector3 center;
	Vector3 extends;
	float radius;
};
class _NexBaseAPI BoundingBox: public AllocMathPool<BoundingBox,
		NEX_BV_POOL_NUM_PER_BLOCK> {
public:

	static BoundingBox Null;

	BoundingBox();

	const Vector3A& GetCenter() const {
		return center;
	}

	const Vector3A& GetExtends() const {
		return extends;
	}

	void SetNull();
	void SetVolume(Vec3AF center, Vec3AF extends);
	void UpdateBounds(Mat4x4F m);
	void UpdateBounds(float scale, QuatF rot, Vec3AF pos);
	void UpdateBounds(const Vector3A* points, uint32 maxPoints);

protected:
	/* center wrt some coordinate system */
	Vector3A center;
	/* half extends dx/2, dy/2, dz/2 and linear radius in w */
	Vector3A extends;
	/* center wrt some coordinate system mostly 0,0,0 */
	Vector3A origCenter;
	/* half extends dx/2, dy/2, dz/2 and linear extends in w */
	Vector3A origExtends;

	friend class BoundingVolume;
};

class _NexBaseAPI BoundingVolume: public AllocGeneral {
	BoundingBox* box;
	float radius;
public:
	static BoundingVolume Null;

	BoundingVolume();
	~BoundingVolume();

	inline const BoundingBox& GetBox() const {
		return *box;
	}

	inline float GetRadius() const {
		return radius;
	}

	inline void SetRadius(float radius) {
		this->radius = radius;
	}

	const Vector3A& GetCenter() const {
		return GetBox().GetCenter();
	}

	const Vector3A& GetExtends() const {
		return GetBox().GetExtends();
	}

	inline void UpdateBounds(Mat4x4F m) {
		box->UpdateBounds(m);
	}

	inline void UpdateBounds(float scale, QuatF rot, Vec3AF pos) {
		box->UpdateBounds(scale, rot, pos);
	}

	inline void UpdateBounds(const Vector3A* points, uint32 maxPoints) {
		box->UpdateBounds(points, maxPoints);
	}

	void SetVolume(Vec3AF center, Vec3AF extends, float radius);
	void MergeBounds(const BoundingVolume&);

};
}

#endif	/* BOUNDING_VOLUME_H */

