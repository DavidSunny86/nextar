/* 
 * File:   Camera.cpp
 * Author: obhi
 * 
 * Created on June 17, 2011, 8:51 AM
 */
#include "NexHeaders.h"
#include "Camera.h"

namespace nextar {

	Camera::Camera(bool allocMatBuff) :
			cameraMatrix(0), visibilityMask(VisibilitySet::VM_ALL) {
		if (allocMatBuff)
			SetCameraMatrix(NEX_NEW CameraMatrix);
		cullFrustum = &viewFrustum;
	}

	Camera::~Camera() {
		viewFrustum.SetPlanes(0, 0);
		if (cameraMatrix)
			NEX_DELETE cameraMatrix;
	}

	void Camera::SetCameraMatrix(CameraMatrix* projectionView) {
		this->cameraMatrix = projectionView;
		/* The frustum planes are allocated by cam buffer */
		viewFrustum.SetPlanes(cameraMatrix->camPlanes, 6);
	}

	void Camera::SetPerspective(float fov, float ratio, float znear, float zfar) {
		fieldOfView = fov;
		aspectRatio = ratio;
		nearDist = znear;
		farDist = zfar;
		orthographic = false;
		cameraMatrix->projection = Mat4x4FromPerspective(fov, ratio, znear,
				zfar);
		SetCornersOutOfDate(true);
		SetViewDimOutOfDate(true);
		InvalidateView();
	}

	void Camera::SetOrthographic(float width, float height, float znear, float zfar) {
		viewWidth = width;
		viewHeight = height;
		nearDist = znear;
		farDist = zfar;
		orthographic = true;
		cameraMatrix->projection = Mat4x4FromOrtho(width, height, znear, zfar);
		SetCornersOutOfDate(true);
		SetViewDimOutOfDate(true);
		InvalidateView();
	}

	const Vector3A* Camera::GetCorners() {
		Vector3A* transCorners = cameraMatrix->corners;
		if (IsBoundsOutOfDate()) {
			/* Recalculate */
			if (IsViewDimOutOfDate())
				_CalculateViewDimensions();
			if (IsCornersOutOfDate())
				_CalculateBoundCorners();
			/* transform bounds */
			Vector3A* corners = cameraMatrix->projCorners;

			for (int i = 0; i < 8; ++i)
				transCorners[i] = Mat4x4TransVec3A(corners[i], *invView);
			SetBoundsOutOfDate(false);
		}
		return transCorners;
	}

	void Camera::_CalculateViewDimensions() {
		if (IsOrthographic()) {
			widthNear = widthFar = viewWidth;
			heightNear = heightFar = viewHeight;
		} else {
			float tanFov = Math::Tan(fieldOfView * 0.5f);
			widthNear = tanFov * nearDist;
			widthFar = tanFov * farDist;
			float invAspect = 1 / aspectRatio;
			heightNear = widthNear * invAspect;
			heightFar = widthFar * invAspect;
		}
		SetViewDimOutOfDate(false);
	}

	void Camera::_CalculateBoundCorners() {

		Vector3A* corners = cameraMatrix->projCorners;
		/*
		 * fc = p + d * farDist
		 * ftl = fc + (up * Hfar/2) - (right * Wfar/2)
		 * ftr = fc + (up * Hfar/2) + (right * Wfar/2)
		 * fbl = fc - (up * Hfar/2) - (right * Wfar/2)
		 * fbr = fc - (up * Hfar/2) + (right * Wfar/2)
		 * 
		 * nc = p + d * nearDist
		 * ntl = nc + (up * Hnear/2) - (right * Wnear/2)
		 * ntr = nc + (up * Hnear/2) + (right * Wnear/2)
		 * nbl = nc - (up * Hnear/2) - (right * Wnear/2)
		 * nbr = nc - (up * Hnear/2) + (right * Wnear/2)
		 *
		 * In projection space:
		 * d = (0,0,1) p = (0,0,0) right = (1,0,0) up = (0,1,0)
		 * Thus:
		 * fc = (0,0,0) + (0,0,1) * farDist = (0,0,farDist)
		 * nc = (0,0,0) + (0,0,1) * nearDist = (0,0,nearDist)
		 * ftl = (-widthFar, heightFar, farDist)
		 * ftr = ( widthFar, heightFar, farDist)
		 * fbl = (-widthFar,-heightFar, farDist)
		 * fbr = ( widthFar,-heightFar, farDist)
		 */
		corners[0] = Vec3ASet(-widthFar, heightFar, farDist);
		corners[1] = Vec3ASet(widthFar, heightFar, farDist);
		corners[2] = Vec3ASet(-widthFar, -heightFar, farDist);
		corners[3] = Vec3ASet(widthFar, -heightFar, farDist);
		corners[0] = Vec3ASet(-widthNear, heightNear, nearDist);
		corners[1] = Vec3ASet(widthNear, heightNear, nearDist);
		corners[2] = Vec3ASet(-widthNear, -heightNear, nearDist);
		corners[3] = Vec3ASet(widthNear, -heightNear, nearDist);

		SetCornersOutOfDate(false);
	}

	void Camera::UpdateFrustum() {
		cameraMatrix->viewProjection = Mat4x4Mul(cameraMatrix->view,
				cameraMatrix->projection);
		viewFrustum.ConstructFrom(cameraMatrix->viewProjection);
		SetFrustumOutOfDate(false);
	}
}
