/*
 * Scene.h
 *
 *  Created on: 27-Jan-2014
 *      Author: obhi
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <Component.h>
#include <Entity.h>
#include <SceneParametrs.h>

namespace nextar {

	/**
	 * Represents a game world.
	 * */
	class _NexExport Scene: public Referenced<Scene, nextar::Component> {
	public:
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
