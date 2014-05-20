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
		// TODO Auto-generated destructor stub
	}

	/** Scene ******************************/
	SceneAsset::SceneAsset(const StringID name) : Asset(name),
	cullingSystem(nullptr) {
	}

	SceneAsset::~SceneAsset() {
	}

	void SceneAsset::_AddEntity(Entity* entity) {
		sceneEntities.push_back(Bind(entity));
		if (cullingSystem) {
			cullingSystem->AddBody(entity->GetSpatial());
		}
	}

	void SceneAsset::_RemoveEntity(Entity* entity) {
		NEX_ASSERT(entity->GetScene() == this);
		sceneEntities.remove(Bind(entity));
	}

} /* namespace nextar */
