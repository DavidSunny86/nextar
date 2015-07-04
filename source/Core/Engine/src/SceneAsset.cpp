/*
 * Scene.cpp
 *
 *  Created on: 27-Jan-2014
 *      Author: obhi
 */

#include <SceneAsset.h>

namespace nextar {

/** SceneParameters *******************/
SceneParameters::SceneParameters() {
	// TODO Auto-generated constructor stub

}

SceneParameters::~SceneParameters() {
}

/** Scene ******************************/
SceneAsset::SceneAsset(const StringID name, const StringID factory) :
		Asset(name, factory), cullingSystem(nullptr) {
}

SceneAsset::~SceneAsset() {
	NEX_SAFE_DELETE(cullingSystem);
}

void SceneAsset::_AddEntity(Entity* entity) {
	sceneEntities.push_back(Bind(entity));
	if (cullingSystem) {
		Spatial* s = entity->GetSpatial();
		bool isCamera = (Component::GetComponentCatagory(s->GetClassID()) & Camera::Traits::GetCatagory()) == Camera::Traits::GetCatagory();
		if (s) {
			if (!isCamera)
				cullingSystem->AddBody(s);
			s->_SetCullingSystem(cullingSystem);
		}
	}
}

void SceneAsset::_RemoveEntity(Entity* entity) {
	NEX_ASSERT(entity->GetScene() == this);
	if (cullingSystem) {
		Spatial* s = entity->GetSpatial();
		bool isCamera = (Component::GetComponentCatagory(s->GetClassID()) & Camera::Traits::GetCatagory()) == Camera::Traits::GetCatagory();
		if (s) {
			if (!isCamera)
				cullingSystem->RemoveBody(s);
			s->_SetCullingSystem(nullptr);
		}
	}
	EntityPtr e = Bind(entity);
	sceneEntities.remove(e);
	toUpdate.remove(e);
}

void SceneAsset::Update(const FrameTimer& frameTimer) {
	Asset::Update(frameTimer);
	for(auto& e : toUpdate) {
		e->Update(frameTimer);
	}
}

void SceneAsset::UnloadImpl() {
	EntityList emptyList;
	std::swap(sceneEntities, emptyList);
}

uint32 SceneAsset::GetClassID() const {
	return CLASS_ID;
}

void SceneAsset::SetCullingSystem(CullingSystem* cullSystem) {
	if (cullingSystem)
		ResetCullingSystem();
	cullingSystem = cullSystem;
	if (cullingSystem) {
		for (auto& e : sceneEntities) {
			Spatial* _s = e->GetSpatial();
			if (_s) {
				cullingSystem->AddBody(_s);
				_s->_SetCullingSystem(cullingSystem);
			}
		}
	}
}

void SceneAsset::ResetCullingSystem() {
	if (cullingSystem) {
		CullingSystem* doDeleteSystem = cullingSystem;
		DisownCullingSystem();
		NEX_DELETE(doDeleteSystem);
	}
}

void SceneAsset::DisownCullingSystem(bool removeBodies) {
	if (cullingSystem) {
		for (auto& e : sceneEntities) {
			Spatial* _s = e->GetSpatial();
			if (_s) {
				cullingSystem->RemoveBody(_s);
				_s->_SetCullingSystem(nullptr);
			}
		}
		cullingSystem = nullptr;
	}
}

StreamRequest* SceneAsset::CreateStreamRequestImpl(bool load) {
	return NEX_NEW(SceneStreamRequest(this));
}

} /* namespace nextar */
