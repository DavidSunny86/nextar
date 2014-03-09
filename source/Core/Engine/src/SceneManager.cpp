/*
 * SceneManager.cpp
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <SceneManager.h>

namespace nextar {

	SceneManager::SceneManager(const String& name) : AssetManager(name) {
	}

	SceneManager::~SceneManager() {
	}

	ScenePtr SceneManager::AsyncCreateEmptyScene(
		const String& name) {
		Scene* scene = static_cast<Scene*>(AsyncCreate(Scene::CLASS_ID, name));
		/* create a BV culling system */

	}
} /* namespace nextar */
