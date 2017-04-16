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

using namespace Math;
struct _NexBaseAPI BoundsInfo {

	static const BoundsInfo Null;

	Vec3::type center;
	Vec3::type extends;
	float radius;

	inline BoundsInfo& operator += (const BoundsInfo& info) {
		if (info.radius > 0) {
			if (radius <= 0)
				*this = info;
			else {
				Vec3::type a = Vec3::Abs(Vec3::Sub(center, info.center));
				Vec3::type b = Vec3::Add(center, info.center);
				center = Vec3::Mul(b, 0.5f);
				extends = Vec3::Add(a, Vec3::Add(extends, info.extends));
				radius += (info.radius + Vec3::Dot(a, a));
			}
		}
		return *this;
	}

	BoundsInfo() : radius(0), center{ 0,0,0 }, extends{ 0, 0, 0 } {
	}
};

class _NexBaseAPI BoundingBox : public AllocMathPool<BoundingBox,
	NEX_BV_POOL_NUM_PER_BLOCK> {
public:


	BoundingBox();

	const Vec3A::type& GetCenter() const {
		return center;
	}

	const Vec3A::type& GetExtends() const {
		return extends;
	}

	void SetNull();
	void SetVolume(Vec3A::pref center, Vec3A::pref extends);
	void SetVolume(Vec3A::pref center, Vec3A::pref extends, float radius);
	void UpdateBounds(Mat4::pref m);
	void UpdateBounds(float scale, Quat::pref rot, Vec3A::pref pos);
	void UpdateBounds(const Vec3A::type* points, uint32 maxPoints);

protected:
	/* center wrt some coordinate system */
	Vec3A::type center;
	/* half extends dx/2, dy/2, dz/2 and linear radius in w */
	Vec3A::type extends;
	/* center wrt some coordinate system mostly 0,0,0 */
	Vec3A::type origCenter;
	/* half extends dx/2, dy/2, dz/2 and linear extends in w */
	union {
		Vec4::type origExtendsAndRadius;
		struct {
			Vec3::type origExtends;
			float radius;
		};
	};

	friend class BoundingVolume;
};

class _NexBaseAPI BoundingVolume : public AllocGeneral {
	BoundingBox* box;
public:
	static BoundingVolume Null;

	BoundingVolume();
	~BoundingVolume();

	inline const BoundingBox& GetBox() const {
		return *box;
	}

	inline float GetRadius() const {
		return box->radius;
	}

	inline void SetRadius(float radius) {
		box->radius = radius;
	}

	const Vec3A::type& GetCenter() const {
		return GetBox().GetCenter();
	}

	const Vec3A::type& GetExtends() const {
		return GetBox().GetExtends();
	}

	inline void UpdateBounds(Mat4::pref m) {
		box->UpdateBounds(m);
	}

	inline void UpdateBounds(float scale, Quat::pref rot, Vec3A::pref pos) {
		box->UpdateBounds(scale, rot, pos);
	}

	inline void UpdateBounds(const Vec3A::type* points, uint32 maxPoints) {
		box->UpdateBounds(points, maxPoints);
	}

	void SetVolume(Vec3A::pref center, Vec3A::pref extends, float radius);
	void MergeBounds(const BoundingVolume&);
};

}

#endif	/* BOUNDING_VOLUME_H */

