/* 
 * File:   Camera.h
 * Author: obhi
 *
 * Created on June 17, 2011, 8:51 AM
 */

#ifndef CAMERA_H
#define	CAMERA_H

#include <NexBase.h>
#include <Frustum.h>
#include <BoundingVolume.h>
#include <Intersect.h>
#include <VisibilitySet.h>
#include <SceneTraversal.h>
#include <Moveable.h>
#include <Spatial.h>

namespace nextar {

class Camera;

class _NexEngineAPI Camera: public Spatial {
public:

	enum {
		CLASS_ID = Component::CLASS_CAMERA,
		CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	enum {
		FRUSTUM_OUTDATED = Spatial::LAST_FLAG << 0,
		PROJECTION_DIRTY = Spatial::LAST_FLAG << 1,
		BOUNDS_OUTDATED = Spatial::LAST_FLAG << 2,
		VIEW_DIM_OUTDATED = Spatial::LAST_FLAG << 3,
	};

	enum {
		PERSPECTIVE, ASYMMETRIC, ORTHOGRAPHIC,
	};
		
	typedef ComponentTraits<Camera> Traits;
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
		Mat4::type view;
		Mat4::type projection;
		Mat4::type viewProjection;
		Mat4::type invProjection;
		Mat4::type invViewProjection;
		/* frustum corners in projection space */
		Vec3A::type projCorners[8];
		/* frustum corners in world space */
		Vec3A::type corners[8];
		/* typical camera planes, used in frustum */
		Plane::type camPlanes[6];
	};

	Camera(const StringID name, const StringID factory,  Component* parent = nullptr);
	virtual ~Camera();

	/** This is basically the inverse of the world transform */
	inline const Mat4::type& GetViewMatrix();
	/* Projection matrix as determined by the current projection parameters */
	inline const Mat4::type& GetProjectionMatrix();
	/* Pre-multiplied view projection matrix */
	inline const Mat4::type& GetViewProjectionMatrix();
	/* Inverse projection */
	inline const Mat4::type& GetInvProjectionMatrix();
	/* Inverse view projection */
	inline const Mat4::type& GetInvViewProjectionMatrix();

	/* Projection type */
	inline uint8 GetProjectionType() const;
	/* */
	inline const PerspectiveParams& GetPerspectiveParams() const;
	/* */
	inline const OrthographicParams& GetOrthographicParams() const;
	/* */
	inline const AsymmetricParams& GetAsymmetricParams() const;
	/* */
	inline const Frustum& GetFrustum() const;
	inline void SetNearDistance(float d);
	inline void SetFarDistance(float d);

	inline bool IsViewDimOutOfDate() const;
	inline bool IsBoundsOutOfDate() const;
	inline bool IsProjectionDirty() const;

	inline void SetCullMask(uint32 cullMask);
	inline uint32 GetCullMask() const;
	inline void SetVisibilityMask(VisibilityMask visibilityMask);
	inline VisibilityMask GetVisibilityMask() const;
	inline float GetDepth(Vec3A::pref ofPoint);
	inline float GetNormalizedDepth(Vec3A::pref ofPoint);

	/** Visibility tests */
	inline bool IsVisible(const BoundingVolume& vol,
			std::ptrdiff_t& coherencyData) const;
	inline bool IsVisible(const BoundingVolume& vol) const;
	inline bool IsVisible(Vec3A::pref center, float radius) const;

	void UpdateProjection();
	void UpdateFrustum();

	void SetPerspectiveParams(const PerspectiveParams& params);
	void SetOrthographicParams(const OrthographicParams& params);
	void SetAsymmetricParams(const AsymmetricParams& params);

	inline Vec3A::type GetRightDirection() const;
	inline Vec3A::type GetUpDirection() const;
	inline Vec3A::type GetViewDirection() const;
	inline Vec3A::type GetEyePosition() const;

	// The point must be a vector of type (x, y, z, 1)
	// Returned coordinates are normalized to -1, 1 in all axes (ClipSpace).
	inline Vec4::type GetWorldToClipCoord(Vec4::pref point);
	// Coordinates are expected to be normalized between -1, 1 in all axes (ClipSpace).
	inline Vec4::type GetClipToWorldCoord(Vec4::pref point);
	// Returned coordinates in screen/clip space with w divided (NDC)
	inline Vec3A::type GetWorldToScreenCoord(Vec3A::pref point);
	// Get point in world coordinate from clip space (NDC)
	inline Vec3A::type GetScreenToWorldCoord(Vec3A::pref point);

	const Vec3A::type* GetCorners();
	/** @remarks Called to update the render queue with renderable data. */
	virtual void Visit(SceneTraversal & traversal) override;
	/** @remarks Update camera */
	virtual void Update(const FrameTimer& frameTimer);

	/** @brief Get node type */
	virtual uint32 GetClassID() const override;

protected:

	inline void _SetCameraMatrixDataPtr(Camera::Matrix* m) {
		/* The frustum planes are allocated by cam buffer */
		matrixData = m;
		viewFrustum.SetPlanes(m->camPlanes, 6);
	}

	void _CalculateViewDimensions();

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
	VisibilityMask visibilityMask;
	uint32 cullMask;

	Frustum viewFrustum;
	/* Culling system of the area where the camera is located. */
	Matrix* matrixData;
};

/************************************************************/
inline const Mat4::type& Camera::GetViewMatrix() {
	return static_cast<Camera::Matrix*>(matrixData)->view;
}

inline const Mat4::type& Camera::GetProjectionMatrix() {
	return static_cast<Camera::Matrix*>(matrixData)->projection;
}

inline const Mat4::type& Camera::GetViewProjectionMatrix() {
	return static_cast<Camera::Matrix*>(matrixData)->viewProjection;
}

inline const Mat4::type& Camera::GetInvProjectionMatrix() {
	return static_cast<Camera::Matrix*>(matrixData)->invProjection;
}

inline const Mat4::type& Camera::GetInvViewProjectionMatrix() {
	return static_cast<Camera::Matrix*>(matrixData)->invViewProjection;
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

inline bool Camera::IsVisible(Vec3A::pref center, float radius) const {
	return Intersect::BoundingSphereFrustum(center, radius, GetFrustum())
			!= Intersect::IR_OUTSIDE;
}

inline bool Camera::IsVisible(const BoundingVolume& vol) const {
	return Intersect::BoundingVolumeFrustum(vol, GetFrustum())
			!= Intersect::IR_OUTSIDE;
}

inline bool Camera::IsVisible(const BoundingVolume& vol,
		std::ptrdiff_t& coherencyData) const {

	uint32 newMask = (coherencyData >> 16) & 0xffff;
	uint32 lastPlane = coherencyData & 0xffff;
	bool intRes = Intersect::BoundingVolumeFrustumCoherent(vol, GetFrustum(),
			cullMask, newMask, lastPlane) != Intersect::IR_OUTSIDE;
	coherencyData = (newMask << 16) | (lastPlane & 0xffff);
	return intRes;
}

inline void Camera::SetNearDistance(float d) {
	nearDistance = d;

	SetFlag(FRUSTUM_OUTDATED | PROJECTION_DIRTY);
}

inline void Camera::SetFarDistance(float d) {
	farDistance = d;

	SetFlag(FRUSTUM_OUTDATED | PROJECTION_DIRTY);
}

inline void Camera::SetCullMask(uint32 cullMask) {
	this->cullMask = cullMask;
}

inline uint32 Camera::GetCullMask() const {
	return cullMask;
}

inline void Camera::SetVisibilityMask(VisibilityMask visibilityMask) {
	this->visibilityMask = visibilityMask;
}

inline VisibilityMask Camera::GetVisibilityMask() const {
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

inline Vec3A::type Camera::GetRightDirection() const {
	return Mat4::Row(*worldMatrix, 0);
}

inline Vec3A::type Camera::GetUpDirection() const {
	return Mat4::Row(*worldMatrix, 1);
}

inline Vec3A::type Camera::GetViewDirection() const {
	return Mat4::Row(*worldMatrix, 2);
}

inline Vec3A::type Camera::GetEyePosition() const {
	return GetTranslation();
}

inline float Camera::GetDepth(Vec3A::pref ofPoint) {
	return Vec3A::SqDistance(ofPoint, GetEyePosition());
}

inline float Camera::GetNormalizedDepth(Vec3A::pref ofPoint) {
	return GetDepth(ofPoint) * recipDistanceInView;
}

inline Vec3A::type Camera::GetWorldToScreenCoord(Vec3A::pref point) {
	return Mat4::Transform(GetViewProjectionMatrix(), point);
}

inline Vec3A::type Camera::GetScreenToWorldCoord(Vec3A::pref point) {
	return Mat4::Transform(GetInvViewProjectionMatrix(), point);
}

inline Vec4::type Camera::GetWorldToClipCoord(Vec4::pref point) {
	return Mat4::Mul(point, GetViewProjectionMatrix());
}

inline Vec4::type Camera::GetClipToWorldCoord(Vec4::pref point) {
	return Mat4::Mul(point, GetInvViewProjectionMatrix());
}

/************************************************************/

}
#endif	/* CAMERA_H */

