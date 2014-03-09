/* 
 * File:   Camera.h
 * Author: obhi
 *
 * Created on June 17, 2011, 8:51 AM
 */

#ifndef CAMERA_H
#define	CAMERA_H

#include "NexSys.h"
#include "Frustum.h"
#include "BoundingVolume.h"
#include "Intersect.h"
#include "VisibilitySet.h"
#include "SceneTraversal.h"
#include "Moveable.h"
#include "Spatial.h"

namespace nextar {

	class Camera;

	class _NexEngineExport Camera : public Spatial {
	public:

		enum {
			CLASS_ID = Component::CLASS_CAMERA,
			CATAGORY = COMPONENT_CAT(CLASS_ID),
		};

		enum {
			FRUSTUM_OUTDATED = Moveable::LAST_FLAG << 0,
			PROJECTION_DIRTY = Moveable::LAST_FLAG << 1,
			BOUNDS_OUTDATED = Moveable::LAST_FLAG << 2,
			VIEW_DIM_OUTDATED = Moveable::LAST_FLAG << 3,
		};

		enum {
			PERSPECTIVE,
			ASYMMETRIC,
			ORTHOGRAPHIC,
		};

		// Symmetric camera
		struct PerspectiveParams {
			float fieldOfView;
			float aspectRatio;
		};

		struct AsymmetricParams {
			float nearLeft, nearTop;
			float nearRight, nearBottom;
			float farLeft, farTop;
			float farRight, farBottom;
		};

		struct OrthographicParams {
			float viewWidth;
			float viewHeight;
		};

		struct Matrix: public AllocMathCore {
			Matrix4x4 view;
			Matrix4x4 projection;
			Matrix4x4 viewProjection;
			/* frustum corners in projection space */
			Vector3A projCorners[8];
			/* frustum corners in world space */
			Vector3A corners[8];
			/* typical camera planes, used in frustum */
			Plane camPlanes[6];
		};

		Camera(const String& name, Component* parent = nullptr);
		virtual ~Camera();

		/** This is basically the inverse of the world transform */
		const Matrix4x4& GetViewMatrix();
		/* Projection matrix as determined by the current projection parameters */
		const Matrix4x4& GetProjectionMatrix();
		/* Pre-multiplied view projection matrix */
		const Matrix4x4& GetViewProjectionMatrix();
		/* Projection type */
		uint8 GetProjectionType() const;
		/* */
		const PerspectiveParams& GetPerspectiveParams() const;
		/* */
		const OrthographicParams& GetOrthographicParams() const;
		/* */
		const AsymmetricParams& GetAsymmetricParams() const;
		/* */
		const Frustum& GetFrustum() const;
		void SetPerspectiveParams(const PerspectiveParams& params);
		void SetOrthographicParams(const OrthographicParams& params);
		void SetAsymmetricParams(const AsymmetricParams& params);
		void SetNearDistance(float d);
		void SetFarDistance(float d);

		inline bool IsViewDimOutOfDate() const;
		inline bool IsBoundsOutOfDate() const;
		inline bool IsProjectionDirty() const;

		void	SetCullMask(uint32 cullMask);
		uint32	GetCullMask() const;
		void	SetVisibilityMask(uint32 visibilityMask);
		uint32	GetVisibilityMask() const;
		float	GetDepth(Vec3AF ofPoint);
		float	GetNormalizedDepth(Vec3AF ofPoint);

		/** Visibility tests */
		bool IsVisible(const BoundingVolume& vol, std::ptrdiff_t& coherencyData) const;
		bool IsVisible(const BoundingVolume& vol) const;
		bool IsVisible(Vec3AF center, float radius) const;

		void UpdateProjection();
		void UpdateFrustum();

		Vector3A GetRightDirection() const;
		Vector3A GetUpDirection() const;
		Vector3A GetViewDirection() const;
		Vector3A GetEyePosition() const;

		const Vector3A* GetCorners();
				/** @remarks Called to update the render queue with renderable data. */
		virtual void Visit(SceneTraversal & traversal) override;

		/** @brief Get node type */
		virtual uint32 GetClassID() const override;
		
	protected:

		inline void _SetCameraMatrixDataPtr(Camera::Matrix* m) {
			/* The frustum planes are allocated by cam buffer */
			viewFrustum.SetPlanes(m->camPlanes, 6);
		}

		void _CalculateViewDimensions();
	
		/* true if this is the main camera */
		uint8 projectionType;

		uint16 viewMatrixNumber;
		uint16 projectMatrixNumber;

		union {
			PerspectiveParams perspective;
			OrthographicParams orthographic;
			AsymmetricParams asymmetric;
		};

		float nearDistance;
		float farDistance;
		float distanceInView;
		float recipDistanceInView;

		/* Mask used to mark objects that needs to be rendered via this
		 * camera.
		 */
		uint32 visibilityMask;
		uint32 cullMask;

		Frustum viewFrustum;
		/* Culling system of the area where the camera is located. */
		Matrix* matrixData;
	};

	/************************************************************/
	inline const Matrix4x4& Camera::GetViewMatrix() {
		return static_cast<Camera::Matrix*>(matrixData)->view;
	}

	inline const Matrix4x4& Camera::GetProjectionMatrix() {
		return static_cast<Camera::Matrix*>(matrixData)->projection;
	}

	inline const Matrix4x4& Camera::GetViewProjectionMatrix() {
		return static_cast<Camera::Matrix*>(matrixData)->viewProjection;
	}

	inline uint8 Camera::GetProjectionType() const {
		return projectionType;
	}

	const Camera::PerspectiveParams& Camera::GetPerspectiveParams() const {
		NEX_ASSERT(projectionType == Camera::PERSPECTIVE);
		return this->perspective;
	}

	const Camera::OrthographicParams& Camera::GetOrthographicParams() const {
		NEX_ASSERT(projectionType == Camera::ORTHOGRAPHIC);
		return this->orthographic;
	}

	const Camera::AsymmetricParams& Camera::GetAsymmetricParams() const {
		NEX_ASSERT(projectionType == Camera::ASYMMETRIC);
		return this->asymmetric;
	}
	
	inline const Frustum& Camera::GetFrustum() const {
		return viewFrustum;
	}

	inline bool Camera::IsVisible(Vec3AF center, float radius) const {
		return Intersect::BoundingSphereFrustum(center, radius, GetFrustum())
				!= Intersect::IR_OUTSIDE;
	}

	inline bool Camera::IsVisible(const BoundingVolume& vol) const {
		return Intersect::BoundingVolumeFrustum(vol, GetFrustum())
				!= Intersect::IR_OUTSIDE;
	}

	inline bool Camera::IsVisible(const BoundingVolume& vol, std::ptrdiff_t& coherencyData) const {
		
		uint32 newMask = (coherencyData >> 16) & 0xffff;
		uint32 lastPlane = coherencyData & 0xffff;
		bool intRes = Intersect::BoundingVolumeFrustumCoherent(vol, GetFrustum(),
				cullMask, newMask, lastPlane)
				!= Intersect::IR_OUTSIDE;
		coherencyData = (newMask << 16) | (lastPlane & 0xffff);
		return intRes;
	}

	inline void Camera::SetNearDistance(float d) {
		nearDistance = d;

		SetFlag(FRUSTUM_OUTDATED|PROJECTION_DIRTY);
	}

	inline void Camera::SetFarDistance(float d) {
		farDistance = d;

		SetFlag(FRUSTUM_OUTDATED|PROJECTION_DIRTY);
	}

	inline void Camera::SetCullMask(uint32 cullMask) {
		this->cullMask = cullMask;
	}

	inline uint32 Camera::GetCullMask() const {
		return cullMask;
	}

	inline void Camera::SetVisibilityMask(uint32 visibilityMask) {
		this->visibilityMask = visibilityMask;
	}

	inline uint32 Camera::GetVisibilityMask() const {
		return visibilityMask;
	}

	inline bool Camera::IsProjectionDirty() const {
		return IsFlagSet(PROJECTION_DIRTY);
	}

	inline bool Camera::IsBoundsOutOfDate() const {
		return IsFlagSet(BOUNDS_OUTDATED);
	}

	inline bool Camera::IsViewDimOutOfDate() const {
		return IsFlagSet(VIEW_DIM_OUTDATED);
	}

	inline Vector3A Camera::GetRightDirection() const {
		return Mat4x4Row(*worldMatrix, 0);
	}

	inline Vector3A Camera::GetUpDirection() const {
		return Mat4x4Row(*worldMatrix, 1);
	}

	inline Vector3A Camera::GetViewDirection() const {
		return Mat4x4Row(*worldMatrix, 2);
	}

	inline Vector3A Camera::GetEyePosition() const {
		return GetPosition();
	}

	inline float Camera::GetDepth(Vec3AF ofPoint) {
		return Vec3ASqDistance(ofPoint, GetEyePosition());
	}

	inline float Camera::GetNormalizedDepth(Vec3AF ofPoint) {
		return GetDepth(ofPoint) * recipDistanceInView;
	}
	/************************************************************/

}
#endif	/* CAMERA_H */

