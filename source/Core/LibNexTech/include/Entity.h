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

	class _NexExport EntityManager : public ComponentManagerImpl {
	public:
		EntityManager(const String& name);

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
	public:
		enum {
			TYPE = Component::COMPONENT_ENTITY,
		};

		Entity(ComponentManager* creator, const String& name);
		virtual ~Entity();

		inline void SetRenderable(Renderable* renderable) {
			this->renderable = renderable;
		}

		inline void SetLight(Light* light) {
			this->light = light;
		}

		/** @brief Get node type */
		virtual int GetType() const;
		/** @brief Visit renderables attached to this node **/
		virtual void FindVisibles(SceneTraversal& traversal);

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
