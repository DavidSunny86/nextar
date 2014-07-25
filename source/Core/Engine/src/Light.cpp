/*
 * Light.cpp
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#include <NexEngine.h>
#include <Light.h>
#include <LightSystem.h>
#include <MeshServices.h>

namespace nextar {

Light::Light(const StringID name) :
		lightType(Type::OMNI),
		lightRange(1),
		falloffAngle(0),
		Renderable(name), sortKey(0) {
	lightVolume.SetWorldMatrices(&GetWorldMatrix(), 1);
	lightVolume.SetBoundsInfo(&GetBoundsInfo());
	lightVolume.SetStreamData( MeshServices::Instance().GetUnitOmniVolume() );
}

Light::~Light() {
}

uint32 Light::GetClassID() const {
	return CLASS_ID;
}

void Light::Visit(SceneTraversal& traversal) {
	traversal.lightSystem->PushLight(sortKey, this);
}

void Light::SetLightRange(float range) {
	Moveable* moveable = GetMoveable();
	if (moveable) {
		moveable->SetScaling(range);
		moveable->UpdateTransform();
	} else {
		float ratio = range / lightRange;
		Matrix4x4& m = GetWorldMatrix();
		m = Mat4x4Scale(ratio, m);
	}
	lightRange = range;
	GetBoundsInfo().SetRadius(lightRange);
	SetBoundsDirty(true);
}

void Light::SetMoveable(Moveable* ptr) {
	Renderable::SetMoveable(ptr);
	Moveable* moveable = GetMoveable();
	if (moveable) {
		moveable->SetScaling(lightRange);
		moveable->UpdateTransform();
	}
	// pointer might change
	lightVolume.SetWorldMatrices(&GetWorldMatrix(), 1);
}

void Light::SetMaterial(uint32 index, MaterialAssetPtr& mtl) {
	lightVolume.SetMaterial(mtl);
}

} /* namespace nextar */
