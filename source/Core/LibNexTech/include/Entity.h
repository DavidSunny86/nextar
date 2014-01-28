/*
 * Entity.h
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include "NexSys.h"
#include "Component.h"

namespace nextar {

	/**
	 * This is a default manager for most/all default scene components.
	 * Any extended component could register its own factory.
	 * */
	class _NexExport EntityManager : public ComponentManagerImpl {
	public:
		EntityManager(const String& name);

		/**
		 * Creates a simple perspective camera pointing in the negetive z direction
		 * 5 units from origin with FOV=45 degrees and aspect ratio = 1.3 */
		virtual EntityPtr AsyncCreateCameraEntity(const String& name);
		virtual EntityPtr AsyncCreateMeshEntity(const String& name, MeshAssetPtr mesh);
		virtual EntityPtr AsyncCreateLightEntity(const String& name);

		/* implementation */
		virtual Component* AsyncCreate(int type, const String& name);
		virtual void AsyncDestroy(Component*);

	protected:
		virtual Component* AsyncCreateImpl(int type, const String& name);
	};

	class _NexExport Entity: public Referenced<Entity, nextar::Component> {
		NEX_LOG_HELPER(Entity);
	public:
		enum {
			TYPE = Component::TYPE_ENTITY,
			CATAGORY = Component::CAT_ENTITY,
		};

		enum {
			IS_CAMERA = Component::LAST_FLAG << 1,
			LAST_FLAG = Component::LAST_FLAG << 2,
		};

		Entity(ComponentManager* creator, const String& name);
		virtual ~Entity();

		/** Attach */
		virtual void AttachComponent(Component* component);
		virtual void DetachComponent(int catagory);
		/** @brief Get node type */
		virtual int GetComponentType() const;
		virtual int GetComponentCatagory() const;

		/** @brief Visit renderables attached to this node **/
		virtual void FindVisibles(SceneTraversal& traversal);

		/* @brief Add entity to scene */
		virtual void AddToScene(Scene*);
		/** @brief Detach the entity from current scene and also optionally
		 * remove it from entity manager, the entity will still exist to the
		 * caller, which can dereference it for proper destruction.
		 * The actual deletion of the entity from memory is not determined. */
		virtual void RemoveFromScene(bool removeFromCreator = true);

	protected:

		Moveable* moveable;
		Renderable* renderable;
		Light* light;
	};

} /* namespace nextar */
#endif /* ENTITY_H_ */
