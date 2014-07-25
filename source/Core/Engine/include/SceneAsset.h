/*
 * SceneAsset.h
 *
 *  Created on: 27-Jan-2014
 *      Author: obhi
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <Asset.h>
#include <Entity.h>
#include <CullingSystem.h>

namespace nextar {

class SceneParameters: public AllocGeneral {
public:
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
class _NexEngineAPI SceneAsset: public Asset {
public:
	enum {
		CLASS_ID = Component::CLASS_SCENE, 
		CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	typedef list<EntityPtr>::type EntityList;
	typedef AssetTraits<SceneAsset> Traits;
	typedef FactoryTraits<SceneAsset> FactoryTraits;

	class SceneStreamRequest: public AssetStreamRequest {
	};

	SceneAsset(const StringID name);
	virtual ~SceneAsset();

	virtual uint32 GetClassID() const;

	/* internal API */
	virtual void _AddEntity(Entity* entity);
	virtual void _RemoveEntity(Entity* entity);

protected:

	virtual void UnloadImpl();

	SceneParameters parameters;
	CullingSystem* cullingSystem;
	EntityList sceneEntities;
};

} /* namespace nextar */
#endif /* SCENE_H_ */
