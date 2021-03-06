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

	Vec3::type maxPoint;
	Vec3::type minPoint;
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
	public:
		SceneStreamRequest(SceneAsset* asset) : AssetStreamRequest(asset) {
		}
	};

	SceneAsset(const StringID name, const StringID factory);
	virtual ~SceneAsset();

	virtual uint32 GetClassID() const;

	virtual void DisownCullingSystem(bool removeBodies = false);
	virtual void SetCullingSystem(CullingSystem* cullSystem);
	virtual void ResetCullingSystem();
	/* internal API */
	virtual void _AddEntity(Entity* entity);
	virtual void _RemoveEntity(Entity* entity);

	virtual void Update(const FrameTimer& frameTimer) override;

	virtual Component* FindComponent(const StringID entity) override;
protected:

	virtual StreamRequest* CreateStreamRequestImpl(bool load);
	virtual void UnloadImpl();

	SceneParameters parameters;
	CullingSystem* cullingSystem;
	EntityList toUpdate;
	EntityList sceneEntities;
};

} /* namespace nextar */
#endif /* SCENE_H_ */
