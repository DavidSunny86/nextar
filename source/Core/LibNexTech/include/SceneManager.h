/*
 * SceneManager.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include "NexSys.h"
#include "Component.h"
#include "scene/RegionNode.h"
#include "scene/MeshNode.h"

namespace nextar {

	class SceneManager: public nextar::ComponentManager {
	public:
		SceneManager();
		virtual ~SceneManager();

		virtual RegionNode* AddRegion(const String& name);
		virtual MeshNode* AddMesh(const String& name, MeshPtr& mesh);
		
	protected:
		
	};

} /* namespace nextar */
#endif /* SCENEMANAGER_H_ */
