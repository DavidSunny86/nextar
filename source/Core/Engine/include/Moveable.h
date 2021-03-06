/* 
 * File:   nexnode.h
 * Author: obhi
 *
 * Created on January 24, 2011, 7:53 AM
 */

#ifndef NEXTAR_MOVEABLE_H_
#define	NEXTAR_MOVEABLE_H_

#include <NexBase.h>
#include <BoundingVolume.h>
#include <Frustum.h>
#include <Component.h>
#include <TransformData.h>

namespace nextar {

/**
 **/
class _NexEngineAPI Moveable: public Component {
	NEX_LOG_HELPER(Moveable)
	;
public:
	enum {
		CLASS_ID = Component::CLASS_MOVABLE,
		CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	enum {
		/** flags set by default */
		DEFAULT_FLAGS = 0, LAST_FLAG = Component::LAST_FLAG << 0,
	};

	typedef ComponentTraits<Moveable> Traits;

	Moveable(const StringID name, const StringID factory,  Component* parent =
			nullptr);
	virtual ~Moveable();

	/** @remarks Returns the local rotation for parent */
	inline const TransformData& GetTransform() const {
		return *transform;
	}

	/** @remarks Returns the local position for parent */
	inline Vec3A::pref GetTranslation() const {
		return transform->GetTranslation();
	}

	/** @remarks Returns the local rotation for parent */
	inline Vec3A::pref GetRotation() const {
		return transform->GetRotation();
	}

	/** @remarks Returns the local scaling for parent */
	inline float GetScaling() const {
		return transform->GetScaling();
	}

	/** @remarks Sets the rotation of object in world space
	 * NotifyUpdated must be called after this call.
	 */
	inline void SetRotation(Quat::pref q) {
		transform->SetRotation(q);
		transform->SetMatrixDirty(true);
		SetUpdateRequired(true);
	}

	/** @remarks Sets the position of object in world space
	 * NotifyUpdated must be called after this call.
	 */
	inline void SetTranslation(Vec3A::pref v) {
		transform->SetTranslation(v);
		transform->SetMatrixDirty(true);
		SetUpdateRequired(true);
	}

	/** @remarks Sets the scaling for object
	 * NotifyUpdated must be called after this call.
	 */
	inline void SetScaling(float s) {
		transform->SetScaling(s);
		transform->SetMatrixDirty(true);
		SetUpdateRequired(true);
	}

	/** @remarks Returns the derived matrix  */
	inline Mat4::type& GetWorldMatrix() {
		if (transform->IsMatrixDirty()) {
			transform->UpdateMatrix();
		}
		return transform->GetMatrix();
	}

	inline void SetTransformDirty() {
		transform->SetMatrixDirty(true);
	}

	inline uint32 GetMatrixState() const {
		return transform->GetMatrixState();
	}

	inline void SetTransform(Vec3A::pref t, Quat::pref q, float scale) {
		transform->SetTranslation(t);
		transform->SetRotation(q);
		transform->SetScaling(scale);
		transform->SetMatrixDirty(true);
		SetUpdateRequired(true);
	}

	inline void TranslateBy(Vec3A::pref t) {
		transform->SetTranslation(Vec3A::Add(t, transform->GetTranslation()));
		transform->SetMatrixDirty(true);
		SetUpdateRequired(true);
	}

	/** @todo Test this function */
	void LocalApplyCameraMotion(const Vec2::type& moveXZ, const Vec2::type& rotateXY);
	/** @todo Test this function */
	void LocalApplyCameraMotionOnLocal(const Vec2::type& moveXZ, const Vec2::type& rotateXY);
	/** @todo Test this function */
	void LocalApplyDeltaTransform(Vec3A::pref t, Quat::pref q);

	virtual void Update(const FrameTimer& frameTimer);
	/** @brief Initialize position, rotation and scaling values to identity */
	void SetIdentityTransforms();
	/* child notifications, received by regions or parent regions */
	virtual void NotifyNodeAdded(Moveable*);
	/* child notifications, received by regions or parent regions */
	virtual void NotifyNodeRemoved(Moveable*);
	/** @brief Specify that states were updated */
	virtual void NotifyUpdated();
	/** @brief Get node type */
	virtual uint32 GetClassID() const;

protected:
	/** Matrices and bounds information */
	TransformData* transform;
};

typedef list<Moveable*>::type MoveableComponentList;
}

#endif	/* NEXTAR_MOVEABLE_H_ */

