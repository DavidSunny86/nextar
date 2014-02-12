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
#include "Asset.h"

namespace nextar {

	class SceneManager: public nextar::AssetManager {
	public:
		SceneManager(const String& name);
		virtual ~SceneManager();
		
		/* Create a scene with a default culling system */
		virtual ScenePtr AsyncCreateEmptyScene(const String& name);
		
	protected:
		
		virtual Component* AsyncCreateImpl(uint32 type, const String& name) override;
	};

} /* namespace nextar */
#endif /* SCENEMANAGER_H_ */
