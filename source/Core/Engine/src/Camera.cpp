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

Camera::Camera(const StringID name, const StringID factory,  Component* parent) :
		Spatial(name, factory, parent), projectMatrixNumber(0),
		visibilityMask(VisibilityMask::VISIBILITY_ALL) {
	Camera::Matrix* matrix = NEX_NEW(Camera::Matrix);
	matrix->view = Mat4::type::IdentityMatrix;
	_SetCameraMatrixDataPtr(matrix);
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
	SetFlag(FRUSTUM_OUTDATED | PROJECTION_DIRTY | BOUNDS_OUTDATED);
}

void Camera::SetOrthographicParams(const Camera::OrthographicParams& params) {
	projectionType = Camera::PERSPECTIVE;
	orthographic = params;
	SetFlag(FRUSTUM_OUTDATED | PROJECTION_DIRTY | BOUNDS_OUTDATED);
	/*m->projection = Mat4x4FromOrtho(width, height, znear, zfar);*/
}

void Camera::SetAsymmetricParams(const Camera::AsymmetricParams& params) {
	projectionType = Camera::ASYMMETRIC;
	asymmetric = params;
	SetFlag(FRUSTUM_OUTDATED | PROJECTION_DIRTY | BOUNDS_OUTDATED);
}

void Camera::UpdateFrustum() {

	Camera::Matrix* m = static_cast<Camera::Matrix*>(matrixData);
	bool updateFrustum = false;
	const Mat4::type& worldData = GetWorldMatrix();
	//if (viewMatrixNumber != GetMatrixNumber()) {

	m->view = Mat4::FromWorldToView(worldData);
	//	viewMatrixNumber = GetMatrixNumber();
	updateFrustum = true;
	//}

	if (IsProjectionDirty()) {
		UpdateProjection();
		updateFrustum = true;
	}

	if (updateFrustum) {
		m->viewProjection = Mat4::Mul(m->view, m->projection);
		m->invViewProjection = Mat4::Inverse(m->viewProjection);
		viewFrustum.ConstructFrom(m->viewProjection);
	}

	UnsetFlag(FRUSTUM_OUTDATED);
}

const Vec3A::type* Camera::GetCorners() {
	Camera::Matrix* m = static_cast<Camera::Matrix*>(matrixData);
	Vec3A::type* transCorners = m->corners;
	const Mat4::type& worldData = GetWorldMatrix();
	if (IsBoundsOutOfDate()) {
		// || viewMatrixNumber != GetMatrixNumber()) {

		/* Recalculate */
		if (IsViewDimOutOfDate())
			_CalculateViewDimensions();
		/* transform bounds */
		Vec3A::type* corners = m->projCorners;

		for (int i = 0; i < 8; ++i)
			transCorners[i] = Mat4::TransformOrtho(worldData, corners[i]);
		UnsetFlag(BOUNDS_OUTDATED);
	}
	return transCorners;
}

void Camera::_CalculateViewDimensions() {

	Camera::Matrix* m = static_cast<Camera::Matrix*>(matrixData);
	Vec3A::type* corners = m->projCorners;
	float leftNear, leftFar;
	float rightNear, rightFar;
	float topNear, topFar;
	float bottomNear, bottomFar;

	switch (projectionType) {
	case ORTHOGRAPHIC:
		leftFar = leftNear = -orthographic.viewWidth;
		rightNear = rightFar = orthographic.viewWidth;
		topNear = topFar = orthographic.viewHeight;
		bottomNear = bottomFar = -orthographic.viewHeight;
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

	corners[0] = Vec3A::Set(leftNear, topNear, nearDistance);
	corners[1] = Vec3A::Set(rightNear, topNear, nearDistance);
	corners[2] = Vec3A::Set(leftNear, bottomNear, nearDistance);
	corners[3] = Vec3A::Set(rightNear, bottomNear, nearDistance);
	corners[4] = Vec3A::Set(leftFar, topFar, farDistance);
	corners[5] = Vec3A::Set(rightFar, topFar, farDistance);
	corners[6] = Vec3A::Set(leftFar, bottomFar, farDistance);
	corners[7] = Vec3A::Set(rightFar, bottomFar, farDistance);

	UnsetFlag(VIEW_DIM_OUTDATED);
}

void Camera::UpdateProjection() {
	distanceInView = farDistance - nearDistance;
	recipDistanceInView = 1 / distanceInView;
	switch (projectionType) {
	case ORTHOGRAPHIC:
		matrixData->projection = Mat4::FromOrthoProjection(orthographic.viewWidth,
				orthographic.viewHeight, nearDistance, farDistance);
		break;
	case PERSPECTIVE:
		matrixData->projection = Mat4::FromPerspectiveProjection(perspective.fieldOfView,
				perspective.aspectRatio, nearDistance, farDistance);
		break;
	case ASYMMETRIC:
		break;
	}
	matrixData->invProjection = Mat4::Inverse(matrixData->projection);
	UnsetFlag(PROJECTION_DIRTY);
}

void Camera::Visit(SceneTraversal & traversal) {
	// @todo Figure out when to update camera
	Update(*traversal.frameTimer);
	Camera* oldCamera = traversal.camera;
	Frustum* oldFrustum = traversal.frustum;
	VisibilityMask oldMask = visibilityMask;
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

void Camera::Update(const FrameTimer& frameTimer) {
	Spatial::Update(frameTimer);
	UpdateFrustum();
}

}
