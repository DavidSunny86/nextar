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
#include "CullingSystem.h"

namespace nextar {
	
	struct SceneParameters : public AllocGeneral {
	
		SceneParameters();
		~SceneParameters();

		Vector3 maxPoint;
		Vector3 minPoint;
	};

	struct SceneEventListener {
	};

	/**
	 * Represents a game world.
	 * */
	class _NexExport Scene: public Asset {
	public:
		enum {
			CLASS_ID = Component::CLASS_SCENE,
			CATAGORY = COMPONENT_CAT(CLASS_ID),
		};

		typedef list<EntityPtr>::type EntityList;

		class SceneStreamRequest : public AssetStreamRequest {
		};

		Scene(AssetManager* creator, const String& name);
		virtual ~Scene();
		
		/* internal API */
		virtual void _AddEntity(Entity* entity);
		virtual void _RemoveEntity(Entity* entity);

	protected:

		virtual void LoadImpl(StreamRequest* req, bool isAsync);
		virtual void UnloadImpl(StreamRequest* req, bool isAsync);

		SceneParameters parameters;
		CullingSystem* cullingSystem;
		EntityList sceneEntities;
	};

} /* namespace nextar */
#endif /* SCENE_H_ */
