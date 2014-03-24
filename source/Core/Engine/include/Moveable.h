/* 
 * File:   nexnode.h
 * Author: obhi
 *
 * Created on January 24, 2011, 7:53 AM
 */

#ifndef NEXTAR_MOVEABLE_H_
#define	NEXTAR_MOVEABLE_H_

#include <NexBase.h>
#include <BoundingVolume.h>
#include <Frustum.h>
#include <Component.h>
#include <TransformData.h>

namespace nextar {
		
	/**
	 **/
	class _NexEngineAPI Moveable: public Component {
		NEX_LOG_HELPER(Moveable);
	public:
		enum {
			CLASS_ID = Component::CLASS_MOVABLE,
			CATAGORY = COMPONENT_CAT(CLASS_ID),
		};

		enum {
			/** flags set by default */
			DEFAULT_FLAGS = 0,
			LAST_FLAG = Component::LAST_FLAG << 0,
		};
				
		Moveable(const String& name = StringUtils::Null, Component* parent = nullptr);
		virtual ~Moveable();

		/** @remarks Returns the local rotation for parent */
		inline const TransformData& GetTransform() const {
			return *transform;
		}

		/** @remarks Returns the local position for parent */
		inline Vec3AF GetTranslation() const {
			return transform->GetTranslation();
		}

		/** @remarks Returns the local position for parent */
		inline Vec3AF GetRotation() const {
			return transform->GetRotation();
		}
		
		/** @remarks Returns the local position for parent */
		inline float GetScaling() const {
			return transform->GetScaling();
		}

		/** @remarks Sets the rotation of object in world space
		 * NotifyUpdated must be called after this call.
		 */
		inline void SetRotation(QuatF q) {
			transform->SetRotation(q);
		}

		/** @remarks Sets the position of object in world space
		 * NotifyUpdated must be called after this call.
		 */
		inline void SetTranslation(Vec3AF v) {
			transform->SetTranslation(v);
		}

		/** @remarks Sets the scaling for object
		 * NotifyUpdated must be called after this call.
		 */
		inline void SetScaling(float s) {
			transform->SetScaling(s);
		}

		/** @remarks Returns the derived matrix  */
		inline Matrix4x4& GetWorldMatrix() {
			if (transform->IsMatrixDirty()) {
				transform->UpdateMatrix();
			}
			return transform->GetMatrix();
		}
	
		/** @brief Initialize position, rotation and scaling values to identity */
		void SetIdentityTransforms();
		/* child notifications, received by regions or parent regions */
		virtual void NotifyNodeAdded(Moveable*);
		/* child notifications, received by regions or parent regions */
		virtual void NotifyNodeRemoved(Moveable*);
		/** @brief Specify that states were updated */
		virtual void NotifyUpdated();
		/** @brief Get node type */
		virtual uint32 GetClassID() const;
		
	protected:
		/** Matrices and bounds information */
		TransformData* transform;
	};

	typedef list<Moveable*>::type MoveableComponentList;
}

#endif	/* NEXTAR_MOVEABLE_H_ */

