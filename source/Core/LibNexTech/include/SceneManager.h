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

namespace nextar {

	class SceneManager: public nextar::ComponentManagerImpl {
	public:
		SceneManager();
		virtual ~SceneManager();

		virtual ScenePtr AsyncCreateEmptyScene(const String& name);
		
	protected:
		
		virtual Component* AsyncCreateImpl(int type, const String& name);
	};

} /* namespace nextar */
#endif /* SCENEMANAGER_H_ */
