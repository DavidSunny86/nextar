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

namespace nextar {

	class Camera;

	class _NexExport Camera {
	public:

		struct CameraMatrix: public AllocMathCore {
			Matrix4x4 view;
			Matrix4x4 projection;
			Matrix4x4 viewProjection;
			/* frustum corners in projection space */
			Vector3A projCorners[8];
			/* frustum corners in world space */
			Vector3A corners[8];
			/* typical camera planes, used in frustum */
			Plane camPlanes[6];
			/* Bounds info for a single scene traversal */
			BoundingVolume boundsInfo;
		};

		Camera(bool allocMatrixBuffer = true);
		virtual ~Camera();

		_NexInline const Matrix4x4* GetViewMatrix() const {
			return &cameraMatrix->view;
		}

		_NexInline const Matrix4x4* GetProjectionMatrix() const {
			return &cameraMatrix->projection;
		}

		_NexInline const Matrix4x4* GetViewProjectionMatrix() const {
			return &cameraMatrix->viewProjection;
		}

		_NexInline BoundingVolume* GetBoundsInfo() {
			return &cameraMatrix->boundsInfo;
		}

		void SetCameraMatrix(CameraMatrix* projectionView);

		CameraMatrix* GetCameraMatrix() const {
			return cameraMatrix;
		}

		void MakeFrustumUptoDate() {
			if (frustumOutOfDate)
				UpdateFrustum();
		}

		_NexInline const Frustum& GetFrustum() const {
			return *cullFrustum;
		}

		_NexInline bool IsMainCamera() const {
			return mainCamera;
		}

		bool IsVisible(const BoundingVolume& vol, uint32& newMask,
				uint32& lastPlane) const {
			uint32 lfp = 0;
			return Intersect::BoundingVolumeFrustumCoherent(vol, GetFrustum(),
					cullMask, &newMask, IsMainCamera() ? &lastPlane : &lfp)
					!= Intersect::IR_OUTSIDE;
		}

		bool IsVisible(const BoundingVolume& vol) const {
			return Intersect::BoundingVolumeFrustum(vol, GetFrustum())
					!= Intersect::IR_OUTSIDE;
		}

		bool IsVisible(Vec3AF center, float radius) const {
			return Intersect::BoundingSphereFrustum(center, radius,
					GetFrustum()) != Intersect::IR_OUTSIDE;
		}

		float GetSqDistance(Vec3AF from) const {
			return Vec3ASqDistance(from, GetEyePosition());
		}

		/* Set projection transform */
		virtual void SetPerspective(float fov, float aspectRatio, float znear,
				float zfar);

		/* Changes camera type and sets orthographic projection */
		virtual void SetOrthographic(float width, float height, float znear,
				float zfar);

		/** @remarks Called to update the render queue with renderable data. */
		virtual void FindVisiblePrimitives(SceneTraversal & traversal) = 0;

		/* invalidate view */
		void InvalidateView() {
			frustumOutOfDate = true;
			boundsOutOfDate = true;
			viewMatrixNumber++;
		}

		const Vector3A* GetCorners();
		void UpdateFrustum();

		void SetCullFrustum(Frustum* cullFrustum) {
			this->cullFrustum = cullFrustum;
		}

		Frustum* GetCullFrustum() const {
			return cullFrustum;
		}

		void SetCullMask(uint32 cullMask) {
			this->cullMask = cullMask;
		}

		uint32 GetCullMask() const {
			return cullMask;
		}

		void SetAspectRatio(float aspectRatio) {
			this->aspectRatio = aspectRatio;
		}

		float GetAspectRatio() const {
			return aspectRatio;
		}

		void SetFieldOfView(float fieldOfView) {
			this->fieldOfView = fieldOfView;
		}

		float GetFieldOfView() const {
			return fieldOfView;
		}

		void SetViewHeight(float viewHeight) {
			this->viewHeight = viewHeight;
		}

		float GetViewHeight() const {
			return viewHeight;
		}

		void SetViewWidth(float viewWidth) {
			this->viewWidth = viewWidth;
		}

		float GetViewWidth() const {
			return viewWidth;
		}

		void SetZFar(float zFar) {
			this->farDist = zFar;
		}

		float GetZFar() const {
			return farDist;
		}

		void SetZNear(float zNear) {
			this->nearDist = zNear;
		}

		float GetZNear() const {
			return nearDist;
		}

		void SetBoundsOutOfDate(bool boundsOutOfDate) {
			this->boundsOutOfDate = boundsOutOfDate;
		}

		bool IsBoundsOutOfDate() const {
			return boundsOutOfDate;
		}

		Vector3A GetRightDirection() const {
			NEX_ASSERT(invView);
			return Mat4x4Row(*invView, 0);
		}

		Vector3A GetUpDirection() const {
			NEX_ASSERT(invView);
			return Mat4x4Row(*invView, 1);
		}

		Vector3A GetViewDirection() const {
			NEX_ASSERT(invView);
			return Mat4x4Row(*invView, 2);
		}

		Vector3A GetEyePosition() const {
			NEX_ASSERT(invView);
			return Mat4x4Row(*invView, 3);
		}

		void SetInvView(Matrix4x4* invView) {
			this->invView = invView;
		}

		const Matrix4x4* GetInvView() const {
			return invView;
		}

		void SetViewMatrixNumber(uint16 viewMatrixNumber) {
			this->viewMatrixNumber = viewMatrixNumber;
		}

		uint16 GetViewMatrixNumber() const {
			return viewMatrixNumber;
		}

		void SetFrustumOutOfDate(bool frustumOutOfDate) {
			this->frustumOutOfDate = frustumOutOfDate;
		}

		bool IsFrustumOutOfDate() const {
			return frustumOutOfDate;
		}

		void SetOrthographic(bool orthographic) {
			this->orthographic = orthographic;
		}

		bool IsOrthographic() const {
			return orthographic;
		}

		void SetCornersOutOfDate(bool cornersOutOfDate) {
			this->cornersOutOfDate = cornersOutOfDate;
		}

		bool IsCornersOutOfDate() const {
			return cornersOutOfDate;
		}

		void SetViewDimOutOfDate(bool viewDimOutOfDate) {
			this->viewDimOutOfDate = viewDimOutOfDate;
		}

		bool IsViewDimOutOfDate() const {
			return viewDimOutOfDate;
		}

		void SetVisibilityMask(uint32 visibilityMask) {
			this->visibilityMask = visibilityMask;
		}

		uint32 GetVisibilityMask() const {
			return visibilityMask;
		}

		void PrepareRendering() {
			cullMask = 0xffffffff;
			MakeFrustumUptoDate();
		}

	protected:

		void _CalculateBoundCorners();
		void _CalculateViewDimensions();

		/* true if this is the main camera */
		bool mainCamera;
		/* true if camera is using orthographic projection */
		bool orthographic;
		/* recalculate frustum */
		bool frustumOutOfDate;
		/* recalculate bounds */
		bool boundsOutOfDate;
		/* projection corners out of date */
		bool cornersOutOfDate;
		/* view dimensions are out of date */
		bool viewDimOutOfDate;

		/* Indicates if view matrix has changed */
		uint16 viewMatrixNumber;

		float nearDist, farDist;
		float viewWidth, viewHeight;
		float fieldOfView;
		float aspectRatio;

		/* Projection params */
		float widthNear, widthFar;
		float heightNear, heightFar;

		/* Mask used to mark objects that needs to be rendered via this
		 * camera.
		 */
		uint32 visibilityMask;
		uint32 cullMask;

		/* This matrix is received from the implementation.
		 * This could be considered as the world matrix of the camera object.
		 */
		const Matrix4x4* invView;

		Frustum viewFrustum;
		/* Culling done via 'viewFrustum' or via a new frustrum created during
		 * portal traversal.
		 */
		Frustum* cullFrustum;
		CameraMatrix* cameraMatrix;
	};
}
#endif	/* CAMERA_H */

