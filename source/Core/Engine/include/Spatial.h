/*
 * Spatial.h
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */

#ifndef SPATIAL_H_
#define SPATIAL_H_

#include <NexBase.h>
#include <Component.h>
#include <SceneTraversal.h>

namespace nextar {

/** 
 * This class is the base class of objects that not only exist in world space, but
 * occupy some volume. It is registered by the collision and culling systems.
 */
class Spatial: public Component {
	NEX_LOG_HELPER(Spatial)
	;
public:

	enum {
		CATAGORY = Component::CAT_SPATIAL
	};

	enum {
		BOUNDS_DIRTY = Component::LAST_FLAG << 1,
		UPDATE_BOUNDS = Component::LAST_FLAG << 2,
		LAST_FLAG = Component::LAST_FLAG << 3
	};
		

	Spatial(const StringID name, const StringID factory,  Component* parent =
			nullptr);
	virtual ~Spatial();

	inline const BoundingVolume& GetBoundsInfo() const {
		return bounds;
	}

	inline CullingSystem* GetCullingSystem() const {
		return culler;
	}

	inline Moveable* GetMoveable() const {
		return moveable;
	}

	inline std::ptrdiff_t& _CullingData() {
		return cullingData;
	}

	/** Internal, used by culling systems only */
	inline void _SetCullingSystem(CullingSystem *culler) {
		this->culler = culler;
	}

	inline bool AreBoundsDirty() const {
		return IsFlagSet(BOUNDS_DIRTY);
	}

	inline void SetBoundsDirty(bool v) {
		SetFlag(BOUNDS_DIRTY, v);
	}

	inline bool IsUpdateBoundsEnabled(bool b) const {
		return IsFlagSet(UPDATE_BOUNDS);
	}

	inline void SetUpdateBoundsEnabled(bool v) {
		SetFlag(UPDATE_BOUNDS, v);
	}

	inline Mat4::type& GetWorldMatrix() {
		return *worldMatrix;
	}

	inline const Mat4::type& GetWorldMatrix() const {
		return *worldMatrix;
	}

	inline Vec3A::type GetTranslation() const {
		return Mat4x4Row(*worldMatrix, 3);
	}

	inline Vec3A::type GetDirectionVector() const {
		return Mat4x4Row(*worldMatrix, 2);
	}

	void SetTransform(Vec3A::pref pos, Quat::pref rot, float scaling) {
		NEX_ASSERT(!moveable);
		*worldMatrix = Mat4x4FromScaleRotPos(scaling, rot, pos);
		SetUpdateRequired(true);
	}

	void SetRadius(float radius) {
		bounds.SetRadius(radius);
		SetBoundsDirty(true);
	}

	inline float GetRadius() const {
		return bounds.GetRadius();
	}

	/** @remarks Update world volume */
	virtual void Update(const FrameTimer& frameTimer);
	virtual void SetMoveable(Moveable* ptr);
	virtual void Visit(SceneTraversal& traversal) = 0;

	virtual uint32 GetClassID() const override = 0;

	virtual void SetMaterial(uint32 index, MaterialAssetPtr& mtl) {}

protected:
	BoundingVolume bounds;
	/* culling system this object belongs to */
	CullingSystem* culler;
	std::ptrdiff_t cullingData;
	/* This is either a part of the moveable or the spatial owns it */
	uint32 matrixState;
	Mat4::type* worldMatrix;
	/* This is a moveable attached to the entity */
	Moveable* moveable;
};
}

#endif //SPATIAL_H_
