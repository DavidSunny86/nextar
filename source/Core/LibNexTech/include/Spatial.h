/*
 * Spatial.h
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */

#ifndef SPATIAL_H_
#define SPATIAL_H_

#include "Component.h"

namespace nextar {

	/** 
	 * This class is the base class of objects that not only exist in world space, but
	 * occupy some volume. It is registered by the collision and culling systems.
	 */
	class Spatial : public Component {
		NEX_LOG_HELPER(Spatial);
	public:

		enum {
			CATAGORY = Component::CAT_SPATIAL
		};

		enum {
			BOUNDS_DIRTY = Component::LAST_FLAG << 1,
			LAST_FLAG = Component::LAST_FLAG << 2
		};

		Spatial(ComponentManager *creator = nullptr, const String& name = StringUtils::Null, Component* parent = nullptr);
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

		inline Matrix4x4* GetWorldMatrix() {
			return worldMatrix;
		}

		inline Vector3A GetPosition() const {
			return Mat4x4Row(*worldMatrix, 3);
		}

		/** @remarks Update world volume */
		void UpdateBounds();
		virtual void SetMoveable(Moveable* ptr);
		virtual void Visit(SceneTraversal& traversal) = 0;

		virtual uint32 GetClassID() const override = 0;
	protected:
		BoundingVolume bounds;
		/* culling system this object belongs to */
		CullingSystem* culler;
		std::ptrdiff_t cullingData;
		/* This is either a part of the moveable or the spatail owns it */
		Matrix4x4* worldMatrix;
		/* This is a moveable attached to the entity */
		Moveable* moveable;
	};
}

#endif //SPATIAL_H_
