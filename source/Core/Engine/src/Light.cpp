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
#include <Moveable.h>

namespace nextar {

Light::Light(const StringID name, const StringID factory, Component* parent) :
		lightType(Type::OMNI),
		lightRange(1),
		falloffAngle(0),
		Renderable(name, factory, parent), sortKey(0) {
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
		moveable->Update();
	} else {
		float ratio = range / lightRange;
		Matrix4x4& m = GetWorldMatrix();
		m = Mat4x4Scale(ratio, m);
	}
	lightRange = range;
	SetRadius(lightRange);
}

void Light::SetMoveable(Moveable* ptr) {
	Renderable::SetMoveable(ptr);
	Moveable* moveable = GetMoveable();
	if (moveable) {
		moveable->SetScaling(lightRange);
		moveable->Update();
	}
	// pointer might change
	lightVolume.SetWorldMatrices(&GetWorldMatrix(), 1);
}

void Light::SetMaterial(uint32 index, MaterialAssetPtr& mtl) {
	lightVolume.SetMaterial(mtl);
}

} /* namespace nextar */
