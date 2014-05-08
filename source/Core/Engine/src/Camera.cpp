/* 
 * File:   Camera.cpp
 * Author: obhi
 * 
 * Created on June 17, 2011, 8:51 AM
 */
#include <NexEngine.h>
#include <Camera.h>
#include <CullingSystem.h>

namespace nextar {

	Camera::Camera(const StringID name, Component* parent) :
		Spatial(name, parent), projectMatrixNumber(0), 
			visibilityMask(VisibilitySet::VM_ALL) {
		_SetCameraMatrixDataPtr(NEX_NEW(Camera::Matrix));
	}

	Camera::~Camera() {
		viewFrustum.SetPlanes(0, 0);
		/** @remarks Expect the derived class to delete it anyway */
		if (matrixData) {
			Camera::Matrix* m = static_cast<Camera::Matrix*>(matrixData);
			NEX_DELETE(m);
			matrixData = nullptr;
		}
	}
	
	void Camera::SetPerspectiveParams(const Camera::PerspectiveParams& params) {
		projectionType = Camera::PERSPECTIVE;
		perspective = params;
		/*m->projection = Mat4x4FromPerspective(perspective.fieldOfView, perspective.aspectRatio, nearDistance,
				farDistance);*/
		SetFlag(FRUSTUM_OUTDATED|PROJECTION_DIRTY|BOUNDS_OUTDATED);
	}

	void Camera::SetOrthographicParams(const Camera::OrthographicParams& params) {
		projectionType = Camera::PERSPECTIVE;
		orthographic = params;
		SetFlag(FRUSTUM_OUTDATED|PROJECTION_DIRTY|BOUNDS_OUTDATED);
		/*m->projection = Mat4x4FromOrtho(width, height, znear, zfar);*/
	}

	void Camera::SetAsymmetricParams(const Camera::AsymmetricParams& params) {
		projectionType = Camera::ASYMMETRIC;
		asymmetric = params;
		SetFlag(FRUSTUM_OUTDATED|PROJECTION_DIRTY|BOUNDS_OUTDATED);
	}

	void Camera::UpdateFrustum() {
		
		Camera::Matrix* m = static_cast<Camera::Matrix*>(matrixData);
		bool updateFrustum = false;
		const Matrix4x4& worldData = GetWorldMatrix();
		//if (viewMatrixNumber != GetMatrixNumber()) {
			
			m->view = Mat4x4ViewFromWorld(worldData);
		//	viewMatrixNumber = GetMatrixNumber();
			updateFrustum = true;
		//}

		if (IsProjectionDirty()) {
			UpdateProjection();
			updateFrustum = true;
		}

		if (updateFrustum) {
			m->viewProjection = Mat4x4Mul(m->view,
				m->projection);
			viewFrustum.ConstructFrom(m->viewProjection);
		}

		UnsetFlag(FRUSTUM_OUTDATED);
	}

	const Vector3A* Camera::GetCorners() {
		Camera::Matrix* m = static_cast<Camera::Matrix*>(matrixData);
		Vector3A* transCorners = m->corners;
		const Matrix4x4& worldData = GetWorldMatrix();
		if (IsBoundsOutOfDate()) { 
			// || viewMatrixNumber != GetMatrixNumber()) {
			
			/* Recalculate */
			if (IsViewDimOutOfDate())
				_CalculateViewDimensions();
			/* transform bounds */
			Vector3A* corners = m->projCorners;

			for (int i = 0; i < 8; ++i)
				transCorners[i] = Mat4x4TransVec3A(corners[i], worldData);
			UnsetFlag(BOUNDS_OUTDATED);
		}
		return transCorners;
	}

	void Camera::_CalculateViewDimensions() {

		Camera::Matrix* m = static_cast<Camera::Matrix*>(matrixData);
		Vector3A* corners = m->projCorners;
		float leftNear, leftFar;
		float rightNear, rightFar;
		float topNear, topFar;
		float bottomNear, bottomFar;

		switch(projectionType) {
		case ORTHOGRAPHIC:
			leftFar = leftNear = -orthographic.viewWidth;
			rightNear = rightFar = orthographic.viewWidth;
			topNear = topFar = orthographic.viewHeight;
			bottomNear = bottomFar =-orthographic.viewHeight;
			break;
		case ASYMMETRIC:
			leftNear = asymmetric.nearLeft;
			leftFar = asymmetric.farLeft;
			
			rightNear = asymmetric.nearRight;
			rightFar = asymmetric.farRight;

			topNear = asymmetric.nearTop;
			topFar = asymmetric.farTop;

			bottomNear = asymmetric.nearBottom;
			bottomFar = asymmetric.farBottom;
			break;
		case PERSPECTIVE:
			float tanFov = Math::Tan(perspective.fieldOfView * 0.5f);
			rightNear = tanFov * nearDistance;
			rightFar = tanFov * farDistance;
			float invAspect = 1 / perspective.aspectRatio;
			topNear = rightNear * invAspect;
			topFar = rightFar * invAspect;
			leftNear = -rightNear;
			leftFar = -rightFar;
			bottomNear = -topNear;
			bottomFar = -topFar;
			break;
		}

		corners[0] = Vec3ASet(leftNear, topNear, nearDistance);
		corners[1] = Vec3ASet(rightNear, topNear, nearDistance);
		corners[2] = Vec3ASet(leftNear, bottomNear, nearDistance);
		corners[3] = Vec3ASet(rightNear, bottomNear, nearDistance);
		corners[4] = Vec3ASet(leftFar, topFar, farDistance);
		corners[5] = Vec3ASet(rightFar, topFar, farDistance);
		corners[6] = Vec3ASet(leftFar, bottomFar, farDistance);
		corners[7] = Vec3ASet(rightFar, bottomFar, farDistance);
	
		UnsetFlag(VIEW_DIM_OUTDATED);
	}
	
	void Camera::UpdateProjection() {
		distanceInView = farDistance - nearDistance;
		recipDistanceInView = 1 / distanceInView;
		switch(projectionType) {
		case ORTHOGRAPHIC:
			matrixData->projection =
					Mat4x4FromOrtho(orthographic.viewWidth, orthographic.viewHeight,
							nearDistance, farDistance);
			break;
		case PERSPECTIVE:
			matrixData->projection =
					Mat4x4FromPerspective(perspective.fieldOfView, perspective.aspectRatio,
							nearDistance, farDistance);
			break;
		case ASYMMETRIC:
			break;
		}
		UnsetFlag(PROJECTION_DIRTY);
	}

	void Camera::Visit(SceneTraversal & traversal) {
		Camera* oldCamera = traversal.camera;
		Frustum* oldFrustum = traversal.frustum;
		uint32 oldMask = visibilityMask;
		traversal.camera = this;
		traversal.visibilityMask = visibilityMask;
		traversal.frustum = &viewFrustum;
		if (culler)
			culler->Visit(traversal);
		traversal.camera = oldCamera;
		traversal.visibilityMask = oldMask;
		traversal.frustum = oldFrustum;
	}
	
	uint32 Camera::GetClassID() const {
		return CLASS_ID;
	}
}
