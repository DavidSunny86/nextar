/*
 * CullingSystem.cpp
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#include <NexEngine.h>
#include <BVCullingSystem.h>
#include <Camera.h>

namespace nextar {

BVCullingSystem::BVCullingSystem(const StringID name, const StringID factory,  Component* parent) :
		CullingSystem(name, factory, parent), coherentCam(nullptr) {
}

BVCullingSystem::~BVCullingSystem() {
}

void BVCullingSystem::AddBody(Spatial* s) {
	NEX_ASSERT(s);
	bodies.push_back(s);
}

void BVCullingSystem::RemoveBody(Spatial* s) {
	NEX_ASSERT(s && s->GetCullingSystem() == this);
	BestErase(bodies, s);
	s->_SetCullingSystem(nullptr);
}

void BVCullingSystem::Visit(SceneTraversal& trav) {
	if (coherentCam == trav.camera) {
		for (auto &b : bodies) {

			if (trav.camera->IsVisible(b->GetBoundsInfo(), b->_CullingData()))
				b->Visit(trav);
		}
	} else {
		for (auto &b : bodies) {
			if (trav.camera->IsVisible(b->GetBoundsInfo()))
				b->Visit(trav);
		}
	}
}

/** @brief Get node type */
uint32 BVCullingSystem::GetClassID() const {
	return CLASS_ID;
}

} /* namespace nextar */
