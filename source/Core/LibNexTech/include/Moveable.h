/* 
 * File:   nexnode.h
 * Author: obhi
 *
 * Created on January 24, 2011, 7:53 AM
 */

#ifndef NEXTAR_MOVEABLE_H_
#define	NEXTAR_MOVEABLE_H_

#include "NexSys.h"
#include "BoundingVolume.h"
#include "Frustum.h"
#include "Component.h"
#include "TransformData.h"

namespace nextar {
		
	/**
	 **/
	class _NexExport Moveable: public Component {
		NEX_LOG_HELPER(Moveable);
	public:
		enum {
			CLASS_ID = Component::CLASS_MOVABLE,
			CATAGORY = COMPONENT_CAT(CLASS_ID),
		};

		enum {
			/** true when matrix needs recalculation */
			MATRIX_DIRTY = Component::LAST_FLAG << 1,
			/** flags set by default */
			DEFAULT_FLAGS = (MATRIX_DIRTY),
			LAST_FLAG = Component::LAST_FLAG << 2,
		};

		typedef TransformData Matrix;
				
		Moveable(ComponentManager *creator,
				const String& name = StringUtils::Null);

		virtual ~Moveable();

		/** @remarks Returns the local rotation for parent */
		inline QuatR GetRotation() const {
			return matrixData->wrot;
		}

		/** @remarks Returns the local position for parent */
		inline Vec3AR GetPosition() const {
			return matrixData->wpos;
		}

		/** @remarks Returns the local position for parent */
		inline float GetScaling() const {
			return matrixData->wscale;
		}

		/** @remarks Sets the rotation of object in world space
		 * NotifyUpdated must be called after this call.
		 */
		inline void SetRotation(QuatF q) {
			matrixData->wrot = q;
		}

		/** @remarks Sets the position of object in world space
		 * NotifyUpdated must be called after this call.
		 */
		inline void SetPosition(Vec3AF v) {
			matrixData->wpos = v;
		}

		/** @remarks Sets the scaling for object
		 * NotifyUpdated must be called after this call.
		 */
		inline void SetScaling(float s) {
			matrixData->wscale = s;
		}
		
		inline void SetMatrixDirty(bool matrixDirty) {
			SetFlag(MATRIX_DIRTY, matrixDirty);
		}

		inline bool IsMatrixDirty() const {
			return (flags & MATRIX_DIRTY) != 0;
		}
		
		inline void SetMatrixNumber(uint16 matrixNumber) {
			this->matrixNumber = matrixNumber;
		}

		inline uint16 GetMatrixNumber() const {
			return matrixNumber;
		}
		
		/** @brief Used internally to store a reference of the existing transform ptr */
		inline Matrix4x4* GetTransformPtr() {
			return &matrixData->cached;
		}

		/** @brief Resets position, rotation and scaling to inital state */
		void SetInitialTransforms();
		/** @brief Initialize position, rotation and scaling values to identity */
		void SetIdentityTransforms();
		/** @remarks Returns the derived matrix  */
		virtual const Matrix4x4& GetFullTransform();
		/* child notifications, received by regions or parent regions */
		virtual void NotifyNodeAdded(Moveable*);
		/* child notifications, received by regions or parent regions */
		virtual void NotifyNodeRemoved(Moveable*);
		/** @brief Specify that states were updated */
		virtual void NotifyUpdated();
		/** @brief Get node type */
		virtual uint32 GetClassID() const;
		
	protected:
		
		/** State number indicating the matrix change state. Every time
		 * the matrix is recalculated this is changed */
		uint16 matrixNumber;
		/* View based info, last frustum plane from the main camera
		 * which rejected this node. 0 is the default. */
		uint16 lastFrustumPlane;
		/** Matrices and bounds information */
		Matrix* matrixData;
	};

	typedef list<Moveable*>::type MoveableComponentList;
}

#endif	/* NEXTAR_MOVEABLE_H_ */

