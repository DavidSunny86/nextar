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

namespace nextar {

	class Camera;

	class _NexExport Camera : public Moveable {
	public:

		enum {
			CATAGORY = Component::CAT_CAMERA,
			TYPE = Component::TYPE_CAMERA
		};

		enum {
			ASSYMETRIC_CAMERA = Moveable::LAST_FLAG << 0,
		};

		enum {
			PROJECTION_PERSPECTIVE,
			PROJECTION_ASYMMETRIC,
			PROJECTION_ORTHOGRAPHIC,
		};

		// Symmetric camera
		struct PerspectiveParams {
			float fieldOfView;
			float aspectRatio;
		};

		struct AsymmetricParams {
			float nearLeft, nearTop;
			float nearRight, nearBottom;
		};

		struct OrthographicParams {
			float viewWidth;
			float viewHeight;
		};

		struct Matrix: public AllocMathCore {
			MatrixBuffer matrixData;

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

		Camera(ComponentManager* creator, const String& name, bool allocMatrixBuffer = true);
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

		void SetCullMask(uint32 cullMask);
		uint32 GetCullMask() const;
		void SetVisibilityMask(uint32 visibilityMask);
		uint32 GetVisibilityMask() const;

		/** Visibility tests */
		bool IsVisible(const BoundingVolume& vol, uint32& newMask,
				uint32& lastPlane) const;
		bool IsVisible(const BoundingVolume& vol) const;
		bool IsVisible(Vec3AF center, float radius) const;

		void UpdateFrustum();

		Vector3A GetRightDirection();
		Vector3A GetUpDirection();
		Vector3A GetViewDirection();
		Vector3A GetEyePosition();

		const Vector3A* GetCorners();
				/** @remarks Called to update the render queue with renderable data. */
		virtual void FindVisiblePrimitives(SceneTraversal & traversal);

		/** @brief Get node type */
		virtual int GetComponentType() const;
		virtual int GetComponentCatagory() const;

	protected:

		void _SetCameraMatrixDataPtr(Matrix* projectionView);
		void _CalculateBoundCorners();
		void _CalculateViewDimensions();

		/* true if this is the main camera */
		uint8 projectionType;
		union {
			PerspectiveParams perspective;
			OrthographicParams orthographic;
			AsymmetricParams asymmetric;
		};

		float nearDistance;
		float farDistance;

		/* recalculate frustum */
		bool frustumOutOfDate;
		/* recalculate bounds */
		bool boundsOutOfDate;
		/* projection corners out of date */
		bool cornersOutOfDate;
		/* view dimensions are out of date */
		bool viewDimOutOfDate;

		/* Projection params */
		float widthNear, widthFar;
		float heightNear, heightFar;

		/* Mask used to mark objects that needs to be rendered via this
		 * camera.
		 */
		uint32 visibilityMask;
		uint32 cullMask;

		Frustum viewFrustum;
		/* Culling system of the area where the camera is located. */
		CullingSystem* culler;
		Matrix* cameraMatrix;
	};

	inline const Matrix4x4& Camera::GetViewMatrix() {
		return cameraMatrix->view;
	}

	inline const Matrix4x4& Camera::GetProjectionMatrix() {
		return &cameraMatrix->projection;
	}

	inline const Matrix4x4& Camera::GetViewProjectionMatrix() {
		return &cameraMatrix->viewProjection;
	}

	inline Camera::Matrix* Camera::GetCameraMatrix() const {
		return cameraMatrix;
	}

	inline void Camera::MakeFrustumUptoDate() {
		if (frustumOutOfDate)
			UpdateFrustum();
	}

	inline const Frustum& Camera::GetFrustum() const {
		return viewFrustum;
	}

	inline bool Camera::IsMainCamera() const {
		return mainCamera;
	}

	inline bool Camera::IsVisible(Vec3AF center, float radius) const {
		return Intersect::BoundingSphereFrustum(center, radius, GetFrustum())
				!= Intersect::IR_OUTSIDE;
	}

	inline bool Camera::IsVisible(const BoundingVolume& vol) const {
		return Intersect::BoundingVolumeFrustum(vol, GetFrustum())
				!= Intersect::IR_OUTSIDE;
	}

	inline bool Camera::IsVisible(const BoundingVolume& vol, uint32& newMask,
			uint32& lastPlane) const {
		uint32 lfp = 0;
		return Intersect::BoundingVolumeFrustumCoherent(vol, GetFrustum(),
				cullMask, &newMask, IsMainCamera() ? &lastPlane : &lfp)
				!= Intersect::IR_OUTSIDE;
	}

	inline float Camera::GetSqDistance(Vec3AF from) const {
		return Vec3ASqDistance(from, GetEyePosition());
	}

	inline void Camera::InvalidateView() {
		frustumOutOfDate = true;
		boundsOutOfDate = true;
		viewMatrixNumber++;
	}

	inline void Camera::SetCullMask(uint32 cullMask) {
		this->cullMask = cullMask;
	}

	inline uint32 Camera::GetCullMask() const {
		return cullMask;
	}

	inline void Camera::SetAspectRatio(float aspectRatio) {
		this->aspectRatio = aspectRatio;
	}

	inline float Camera::GetAspectRatio() const {
		return aspectRatio;
	}

	inline void Camera::SetFieldOfView(float fieldOfView) {
		this->fieldOfView = fieldOfView;
	}

	inline float Camera::GetFieldOfView() const {
		return fieldOfView;
	}

	inline void Camera::SetViewHeight(float viewHeight) {
		this->viewHeight = viewHeight;
	}

	inline float Camera::GetViewHeight() const {
		return viewHeight;
	}

	inline void Camera::SetViewWidth(float viewWidth) {
		this->viewWidth = viewWidth;
	}

	inline float Camera::GetViewWidth() const {
		return viewWidth;
	}

	inline void Camera::SetZFar(float zFar) {
		this->farDist = zFar;
	}

	inline float Camera::GetZFar() const {
		return farDist;
	}

	inline void Camera::SetZNear(float zNear) {
		this->nearDist = zNear;
	}

	inline float Camera::GetZNear() const {
		return nearDist;
	}

	inline void Camera::SetBoundsOutOfDate(bool boundsOutOfDate) {
		this->boundsOutOfDate = boundsOutOfDate;
	}

	inline bool Camera::IsBoundsOutOfDate() const {
		return boundsOutOfDate;
	}

	inline Vector3A Camera::GetRightDirection() {
		return Mat4x4Row(GetInvViewMatrix(), 0);
	}

	inline Vector3A Camera::GetUpDirection() {
		return Mat4x4Row(GetInvViewMatrix(), 1);
	}

	inline Vector3A Camera::GetViewDirection() {
		return Mat4x4Row(GetInvViewMatrix(), 2);
	}

	inline Vector3A Camera::GetEyePosition() {
		return Mat4x4Row(GetInvViewMatrix(), 3);
	}

	inline const Matrix4x4& Camera::GetInvViewMatrix() {
		return GetFullTransform();
	}

	inline void Camera::SetViewMatrixNumber(uint16 viewMatrixNumber) {
		this->viewMatrixNumber = viewMatrixNumber;
	}

	inline uint16 Camera::GetViewMatrixNumber() const {
		return viewMatrixNumber;
	}

	inline void Camera::SetFrustumOutOfDate(bool frustumOutOfDate) {
		this->frustumOutOfDate = frustumOutOfDate;
	}

	inline bool Camera::IsFrustumOutOfDate() const {
		return frustumOutOfDate;
	}

	inline bool Camera::IsOrthographic() const {
		return orthographic;
	}

	inline void Camera::SetCornersOutOfDate(bool cornersOutOfDate) {
		this->cornersOutOfDate = cornersOutOfDate;
	}

	inline bool Camera::IsCornersOutOfDate() const {
		return cornersOutOfDate;
	}

	inline void Camera::SetViewDimOutOfDate(bool viewDimOutOfDate) {
		this->viewDimOutOfDate = viewDimOutOfDate;
	}

	inline bool Camera::IsViewDimOutOfDate() const {
		return viewDimOutOfDate;
	}

	inline void Camera::SetVisibilityMask(uint32 visibilityMask) {
		this->visibilityMask = visibilityMask;
	}

	inline uint32 Camera::GetVisibilityMask() const {
		return visibilityMask;
	}

	inline void Camera::PrepareRendering() {
		cullMask = 0xffffffff;
		MakeFrustumUptoDate();
	}

	inline void Camera::SetOrthographic(bool orthographic) {
		this->orthographic = orthographic;
	}

}
#endif	/* CAMERA_H */

