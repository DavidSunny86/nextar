/*
 * Scene.cpp
 *
 *  Created on: 27-Jan-2014
 *      Author: obhi
 */

#include <Scene.h>

namespace nextar {

	/** SceneParameters *******************/
	SceneParameters::SceneParameters() {
		// TODO Auto-generated constructor stub

	}

	SceneParameters::~SceneParameters() {
		// TODO Auto-generated destructor stub
	}

	/** Scene ******************************/
	Scene::Scene(AssetManager* creator, const String& name) : Asset(creator, name),
	cullingSystem(nullptr) {
	}

	Scene::~Scene() {
	}

	void Scene::_AddEntity(Entity* entity) {
		sceneEntities.push_back(Bind(entity));
		if (cullingSystem) {
			cullingSystem->AddBody(entity->GetSpatial());
		}
	}

	void Scene::_RemoveEntity(Entity* entity) {
		NEX_ASSERT(entity->GetScene() == this);
		sceneEntities.remove(Bind(entity));
	}
} /* namespace nextar */
