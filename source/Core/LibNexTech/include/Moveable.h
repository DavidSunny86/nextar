/* 
 * File:   nexnode.h
 * Author: obhi
 *
 * Created on January 24, 2011, 7:53 AM
 */

#ifndef NEXTAR_NEXNODE_H
#define	NEXTAR_NEXNODE_H

#include "NexSys.h"
#include "BoundingVolume.h"
#include "Frustum.h"
#include "Camera.h"
#include "Component.h"

namespace nextar {

	/**
	 **/
	class _NexExport Moveable: public Component {
	public:
		enum {
			TYPE = TYPE_MOVABLE,
			CATAGORY = CAT_MOVEABLE,
		};

		enum {
			/** true enables bound updates */
			UPDATE_BOUNDS_ENABLED = Component::LAST_FLAG << 1,
			/** true when matrix needs recalculation */
			MATRIX_DIRTY = Component::LAST_FLAG << 5,
			/** flags set by default */
			DEFAULT_FLAGS = (UPDATE_BOUNDS_ENABLED
					| MATRIX_DIRTY  ),
			LAST_FLAG = Component::LAST_FLAG << 11,
		};

		struct MatrixBuffer: public AllocMathCore {
			/* World rotation */
			Quaternion wrot;
			/* World postion */
			Vector3A wpos;
			/* Initial rotation */
			Quaternion irot;
			/* Initial position */
			Vector3A ipos;
			/* Cached matrix */
			Matrix4x4 cached;
			/* Bound information */
			BoundingVolume bounds;
			/* variable placement takes into account the alignment */
			float iscale;
			float wscale;
			/* 4 byte padding */
			uint32 flags;
		};

		NEX_LOG_HELPER(Moveable);

		Moveable(ComponentManager *creator,
				const String& name = StringUtils::Null, bool allocMatrixBuff =
						true);

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

		inline void SetUpdateBoundsEnabled(bool updateBoundsEnabled) {
			SetFlag(UPDATE_BOUNDS_ENABLED, updateBoundsEnabled);
		}

		inline bool IsUpdateBoundsEnabled() const {
			return (flags & UPDATE_BOUNDS_ENABLED) != 0;
		}

		inline void SetMatrixDataPtr(MatrixBuffer* matrixData) {
			this->matrixData = matrixData;
			if (matrixData) {
				SetIdentityTransforms();
			}
		}

		inline MatrixBuffer* GetMatrixDataPtr() const {
			return matrixData;
		}

		inline void SetMatrixNumber(uint16 matrixNumber) {
			this->matrixNumber = matrixNumber;
		}

		inline uint16 GetMatrixNumber() const {
			return matrixNumber;
		}

		inline const BoundingVolume& GetBoundsInfo() const {
			return matrixData->bounds;
		}

		/** @brief Resets position, rotation and scaling to inital state */
		void SetInitialTransforms();
		/** @brief Initialize position, rotation and scaling values to identity */
		void SetIdentityTransforms();
		/** @remarks Update world volume */
		virtual void UpdateBounds();
		/** @remarks Returns the derived matrix  */
		virtual const Matrix4x4& GetFullTransform();
		/* child notifications, received by regions or parent regions */
		virtual void NotifyNodeAdded(Moveable*);
		/* child notifications, received by regions or parent regions */
		virtual void NotifyNodeRemoved(Moveable*);
		/** @brief Specify that states were updated */
		virtual void NotifyUpdated();
		/** @brief Get node type */
		virtual int GetComponentType() const;
		virtual int GetComponentCatagory() const;

	protected:
		/** State number indicating the matrix change state. Every time
		 * the matrix is recalculated this is changed */
		uint16 matrixNumber;
		/* View based info, last frustum plane from the main camera
		 * which rejected this node. 0 is the default. */
		uint16 lastFrustumPlane;
		/** Matrices and bounds information */
		MatrixBuffer* matrixData;
	};

	typedef list<Moveable*>::type MoveableComponentList;
}

#endif	/* NEXTAR_NEXNODE_H */

