#include <NexEngine.h>
#include <Moveable.h>
#include <PooledAllocator.h>

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

void Moveable::LocalApplyCameraMotion(const Vec2::type& moveXZ, 
	const Vec2::type& rotateXY) {
	Quat::type rotation = transform->GetRotation();
	Vec3A::type position = transform->GetTranslation();
	
	Quat::type nrotX = Quat::FromAxisAngle(Vec3::Set(1.0f, 0.0f, 0.0f), rotateXY.x);
	Quat::type nrotY = Quat::FromAxisAngle(Vec3::Set(0.0f, 1.0f, 0.0f), rotateXY.y);
	rotation = Quat::Mul(Quat::Mul(nrotX, rotation), nrotY);
	position = Vec3A::Add(position, Quat::Transform(rotation, Vec3A::Set(moveXZ.x, 0.0f, -moveXZ.y)));
	transform->SetTranslation(position);
	transform->SetRotation(rotation);
	transform->SetMatrixDirty(true);
	SetUpdateRequired(true);
}

void Moveable::LocalApplyCameraMotionOnLocal(const Vec2::type& moveXZ,
	const Vec2::type& rotateXY) {
	Quat::type rotation = transform->GetRotation();
	Vec3A::type position = transform->GetTranslation();

	Quat::type nrotX = Quat::FromAxisAngle(Vec3::Set(1.0f, 0.0f, 0.0f), rotateXY.x);
	Quat::type nrotY = Quat::FromAxisAngle(Vec3::Set(0.0f, 1.0f, 0.0f), rotateXY.y);
	rotation = Quat::Mul(nrotY, Quat::Mul(nrotX, rotation));
	position = Vec3A::Add(position, Quat::Transform(rotation, Vec3A::Set(moveXZ.x, 0.0f, -moveXZ.y)));
	transform->SetTranslation(position);
	transform->SetRotation(rotation);
	transform->SetMatrixDirty(true);
	SetUpdateRequired(true);
}

void Moveable::LocalApplyDeltaTransform(Vec3A::pref t, Quat::pref q) {
	Quat::type r = transform->GetRotation();
	Vec3A::type dt = Quat::Transform(r, t);
	transform->SetTranslation(Vec3A::Add(dt, transform->GetTranslation()));
	transform->SetRotation(Quat::Mul(r, q));
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
