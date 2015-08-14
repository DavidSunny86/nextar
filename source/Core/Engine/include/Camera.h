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
		Matrix4x4 view;
		Matrix4x4 projection;
		Matrix4x4 viewProjection;
<<<<<<< HEAD
=======
		Matrix4x4 invProjection;
		Matrix4x4 invViewProjection;
>>>>>>> 14d8c5bb1272811606af0c2487b2480748153031
		/* frustum corners in projection space */
		Vector3A projCorners[8];
		/* frustum corners in world space */
		Vector3A corners[8];
		/* typical camera planes, used in frustum */
		Plane camPlanes[6];
	};

	Camera(const StringID name, const StringID factory,  Component* parent = nullptr);
	virtual ~Camera();

	/** This is basically the inverse of the world transform */
	inline const Matrix4x4& GetViewMatrix();
	/* Projection matrix as determined by the current projection parameters */
	inline const Matrix4x4& GetProjectionMatrix();
	/* Pre-multiplied view projection matrix */
	inline const Matrix4x4& GetViewProjectionMatrix();
<<<<<<< HEAD
=======
	/* Inverse projection */
	inline const Matrix4x4& GetInvProjectionMatrix();
	/* Inverse view projection */
	inline const Matrix4x4& GetInvViewProjectionMatrix();

>>>>>>> 14d8c5bb1272811606af0c2487b2480748153031
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
	inline float GetDepth(Vec3AF ofPoint);
	inline float GetNormalizedDepth(Vec3AF ofPoint);

	/** Visibility tests */
	inline bool IsVisible(const BoundingVolume& vol,
			std::ptrdiff_t& coherencyData) const;
	inline bool IsVisible(const BoundingVolume& vol) const;
	inline bool IsVisible(Vec3AF center, float radius) const;

	void UpdateProjection();
	void UpdateFrustum();

	void SetPerspectiveParams(const PerspectiveParams& params);
	void SetOrthographicParams(const OrthographicParams& params);
	void SetAsymmetricParams(const AsymmetricParams& params);

	inline Vector3A GetRightDirection() const;
	inline Vector3A GetUpDirection() const;
	inline Vector3A GetViewDirection() const;
	inline Vector3A GetEyePosition() const;

<<<<<<< HEAD
=======
	// The point must be a vector of type (x, y, z, 1)
	// Returned coordinates are normalized to -1, 1 in all axes (ClipSpace).
	inline Vector4A GetWorldToClipCoord(Vector4A point);
	// Coordinates are expected to be normalized between -1, 1 in all axes (ClipSpace).
	inline Vector4A GetClipToWorldCoord(Vector4A point);
	// Returned coordinates in screen/clip space with w divided (NDC)
	inline Vector3A GetWorldToScreenCoord(Vector3A point);
	// Get point in world coordinate from clip space (NDC)
	inline Vector3A GetScreenToWorldCoord(Vector3A point);

>>>>>>> 14d8c5bb1272811606af0c2487b2480748153031
	const Vector3A* GetCorners();
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
inline const Matrix4x4& Camera::GetViewMatrix() {
	return static_cast<Camera::Matrix*>(matrixData)->view;
}

inline const Matrix4x4& Camera::GetProjectionMatrix() {
	return static_cast<Camera::Matrix*>(matrixData)->projection;
}

inline const Matrix4x4& Camera::GetViewProjectionMatrix() {
	return static_cast<Camera::Matrix*>(matrixData)->viewProjection;
}

<<<<<<< HEAD
=======
inline const Matrix4x4& Camera::GetInvProjectionMatrix() {
	return static_cast<Camera::Matrix*>(matrixData)->invProjection;
}

inline const Matrix4x4& Camera::GetInvViewProjectionMatrix() {
	return static_cast<Camera::Matrix*>(matrixData)->invViewProjection;
}

>>>>>>> 14d8c5bb1272811606af0c2487b2480748153031
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
	return GetTranslation();
}

inline float Camera::GetDepth(Vec3AF ofPoint) {
	return Vec3ASqDistance(ofPoint, GetEyePosition());
}

inline float Camera::GetNormalizedDepth(Vec3AF ofPoint) {
	return GetDepth(ofPoint) * recipDistanceInView;
}
<<<<<<< HEAD
=======

inline Vector3A Camera::GetWorldToScreenCoord(Vector3A point) {
	return Mat4x4TransAndProjVec3A(point, GetViewProjectionMatrix());
}

inline Vector3A Camera::GetScreenToWorldCoord(Vector3A point) {
	return Mat4x4TransAndProjVec3A(point, GetInvViewProjectionMatrix());
}

inline Vector4A Camera::GetWorldToClipCoord(Vector4A point) {
	return Vec4AMulMat4x4(point, GetViewProjectionMatrix());
}

inline Vector4A Camera::GetClipToWorldCoord(Vector4A point) {
	return Vec4AMulMat4x4(point, GetInvViewProjectionMatrix());

}

>>>>>>> 14d8c5bb1272811606af0c2487b2480748153031
/************************************************************/

}
#endif	/* CAMERA_H */

