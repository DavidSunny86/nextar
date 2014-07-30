/*
 * Entity.h
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include <NexBase.h>
#include <Component.h>
#include <Moveable.h>
#include <Camera.h>

namespace nextar {

/** 
 * We expect a lot of entities, so we will override new and delete operators
 * so that they allocate from a singleton pool.
 */
class _NexEngineAPI Entity:
//public AllocPooledObjectBase< PooledAllocator<Entity, 32, MEMCAT_GENERAL> >,
public nextar::SharedComponent {
	NEX_LOG_HELPER(Entity)
	;
public:
	enum {
		CLASS_ID = Component::CLASS_ENTITY, CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	enum {
		LAST_FLAG = Component::LAST_FLAG << 0,
	};

	/**
	 * This is a default manager for most/all default scene components.
	 * Any extended component could register its own factory.
	 * */
	class _NexEngineAPI Factory: public Component::Factory {
		NEX_LOG_HELPER(EntityFactory)
		;
	public:
		Factory(const StringID name);

		/**
		 * Creates a simple perspective camera pointing in the negetive z direction
		 * 5 units from origin with FOV=45 degrees and aspect ratio = 1.3 */
		virtual EntityPtr AsyncCreateCameraEntity(const StringID name);
		virtual EntityPtr AsyncCreateMeshEntity(const StringID name,
				MeshAssetPtr mesh);
		virtual EntityPtr AsyncCreateLightEntity(const StringID name);
		virtual EntityPtr AsyncCreateOmniLightEntity(const StringID name, const Color&, float range);

		/* implementation */
		virtual Component* AsyncCreate(uint32 classId, const StringID name);

	private:
		friend class ComponentFactoryArchive;
		static void _InternalRegisterToArchive();
	protected:

		virtual EntityPtr AsyncCreateAndAttach(const StringID name,
				uint32 subType, const StringID subTypeName,
				Component** subComponent = nullptr);
	};

	Entity(const StringID name, const StringID factory);
	virtual ~Entity();

	inline Spatial* GetSpatial() const {
		return spatial;
	}

	inline Moveable* GetMoveable() const {
		return moveable;
	}

	inline SceneAsset* GetScene() {
		return scene;
	}

	/** Attach */
	virtual void AttachComponent(Component* component);
	virtual void DetachComponent(uint32 type);
	/** @brief Get node type */
	virtual uint32 GetClassID() const override;

	/* @brief Add entity to scene */
	virtual void AddToScene(SceneAsset*);
	/** @brief Detach the entity from current scene and also optionally
	 * remove it from group it is added to, the entity will still exist to the
	 * caller, which can dereference it for proper destruction.
	 * The actual deletion of the entity from memory is not determined. */
	virtual void RemoveFromScene(bool removeFromGroup = true);

	virtual void Update();

	void SetTransform(Vec3AF position, QuatF rotation, float scaling);

protected:

	SceneAsset* scene;
	Moveable* moveable;
	/* Only a single spatial object can be attached to an entity
	 * at a given time. So camera and renderable cannot be attached to the
	 * same entity as both derive from Spatial. */
	Spatial* spatial;
};

} /* namespace nextar */
#endif /* ENTITY_H_ */
