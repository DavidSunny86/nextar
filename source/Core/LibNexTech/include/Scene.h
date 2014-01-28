/*
 * Scene.h
 *
 *  Created on: 27-Jan-2014
 *      Author: obhi
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <Asset.h>
#include <Entity.h>
#include <SceneParametrs.h>

namespace nextar {

	struct SceneEventListener {
	};

	/**
	 * Represents a game world.
	 * */
	class _NexExport Scene: public Asset {
	public:
		enum {
			TYPE = Component::COMPONENT_SCENE,
		};

		typedef list<EntityPtr>::type EntityList;

		Scene();
		virtual ~Scene();


		/* internal API */
		virtual void _AddEntity(Entity* entity);
		virtual void _RemoveEntity(Entity* entity);

	protected:

		SceneParameters parameters;
		EntityList sceneEntities;
	};

} /* namespace nextar */
#endif /* SCENE_H_ */
