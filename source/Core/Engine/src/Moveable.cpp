#include <NexEngine.h>
#include <Moveable.h>
#include <CommonPools.h>

namespace nextar {

Moveable::Moveable(const StringID nodeName, const StringID factory,  Component* parent) :
		Component(nodeName, factory, parent), transform(NEX_NEW(TransformData)) {
	flags = DEFAULT_FLAGS;
	SetIdentityTransforms();
}

Moveable::~Moveable() {
	NEX_DELETE((transform));
	transform = nullptr;
}

void Moveable::LocalApplyCameraMotion(const Vector2& moveXZ, 
	const Vector2& rotateXY) {
	Quaternion rotation = transform->GetRotation();
	Vector3A position = transform->GetTranslation();
	
	Quaternion nrotX = QuatFromAxisAng(Vec3ASet(1.0f, 0.0f, 0.0f), rotateXY.x);
	Quaternion nrotY = QuatFromAxisAng(Vec3ASet(0.0f, 1.0f, 0.0f), rotateXY.y);
	rotation = QuatMul(QuatMul(nrotX, rotation), nrotY);
	position = Vec3AAdd(position, QuatTransVec3A(rotation, Vec3ASet(moveXZ.x, 0.0f, -moveXZ.y)));
	transform->SetTranslation(position);
	transform->SetRotation(rotation);
	transform->SetMatrixDirty(true);
	SetUpdateRequired(true);
}

void Moveable::LocalApplyDeltaTransform(Vec3AF t, QuatF q) {
	Quaternion r = transform->GetRotation();
	Vector3A dt = QuatTransVec3A(r, t);
	transform->SetTranslation(Vec3AAdd(dt, transform->GetTranslation()));
	transform->SetRotation(QuatMul(r, q));
	transform->SetMatrixDirty(true);
	SetUpdateRequired(true);
}
void Moveable::SetIdentityTransforms() {
	NEX_ASSERT(transform);
	transform->SetIdentity();
}

void Moveable::NotifyNodeAdded(Moveable*) {
}

void Moveable::NotifyNodeRemoved(Moveable*) {
}

void Moveable::NotifyUpdated() {
	transform->SetMatrixDirty(true);
}

/** @brief Get node type */
uint32 Moveable::GetClassID() const {
	return CLASS_ID;
}

void Moveable::Update(const FrameTimer& frameTimer) {
	if (transform->IsMatrixDirty())
		transform->UpdateMatrix();
	SetUpdateRequired(false);
}

}
