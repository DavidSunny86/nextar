/*
 * Entity.cpp
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#include "NexHeaders.h"
#include "Entity.h"
#include "Mesh.h"
#include "Light.h"
#include "Moveable.h"
#include "Camera.h"

namespace nextar {

	/*************************************
	 * EntityManager
	 *************************************/
	EntityManager::EntityManager(const String& name) :
			ComponentManagerImpl(name) {
	}

	EntityPtr EntityManager::AsyncCreateCameraEntity(const String& name) {
		EntityPtr ent = Assign(AsyncCreate(Entity::TYPE, name));
		Camera* camera = static_cast<Camera*>(AsyncCreate(Camera::TYPE, name));
		ent->AttachComponent(camera);
		return ent;
	}

	Component* EntityManager::AsyncCreateImpl(uint32 type, const String& name) {
		switch(type) {
		case Entity::TYPE:
			return NEX_NEW Entity(this, name);
		case Light::TYPE:
			return NEX_NEW Light(this, name);
		case Mesh::TYPE:
			return NEX_NEW Mesh(this, name);
		case Moveable::TYPE:
			return NEX_NEW Moveable(this, name);
		case Camera::TYPE:
			return NEX_NEW Camera(this, name);
		}
		return 0;
	}

	/*************************************
	 * Entity
	 *************************************/
	Entity::Entity(ComponentManager* creator, const String& name) : moveable(0),
			renderable(0), light(0) {
		SetCreator(creator);
		SetName(name);
	}

	Entity::~Entity() {
		if (moveable)
			NEX_DELETE moveable;
		if (renderable)
			NEX_DELETE renderable;
	}

	void Entity::FindVisibles(SceneTraversal& traversal) {
		Moveable* lastMoveable = traversal.moveable;
		if (moveable)
			traversal.moveable = moveable;
		if (renderable)
			renderable->Visit(traversal);
		traversal.moveable = lastMoveable;
	}

	void Entity::AttachComponent(Component* comp) {
		uint32 type = comp->GetClassID();
		uint16 catagory = Component::GetComponentCatagory(type);
		if(catagory & Entity::CATAGORY) {
			Error("Sub-entities not supported yet!");
		}

		if(catagory & Renderable::CATAGORY) {
			renderable = static_cast<Renderable*>(comp);
			renderable->NotifyMoveableChanged(moveable);
		}

		if(catagory & Moveable::CATAGORY) {
			if(catagory & Camera::CATAGORY)
				SetFlag(IS_CAMERA);
			moveable = static_cast<Moveable*>(comp);
			if (renderable)
				renderable->NotifyMoveableChanged(moveable);
		}

		comp->SetParent(this);
		/* @todo Fire event */
	}

	void Entity::DetachComponent(uint32 type) {
		Component* toDetach = nullptr;
		uint16 catagory = Component::GetComponentCatagory(type);
		if(catagory & Entity::CATAGORY) {
			Error("Sub-entities not supported yet!");
		}

		if(catagory & Renderable::CATAGORY) {
			NEX_ASSERT(!toDetach || toDetach == static_cast<Component*>(renderable));
			toDetach = renderable;
			renderable = nullptr;
		}

		if(catagory & Moveable::CATAGORY) {
			NEX_ASSERT(!toDetach || toDetach == static_cast<Component*>(moveable));
			toDetach = moveable;
			moveable = nullptr;
			if(catagory & Camera::CATAGORY)
				UnsetFlag(IS_CAMERA);
			if (renderable)
				renderable->NotifyMoveableChanged(nullptr);
		}

		if (toDetach) {
			toDetach->SetParent(nullptr);
			NEX_DELETE toDetach;
		} // else report error ??
	}

	uint32 Entity::GetClassID() const {
		return Entity::CLASS_ID;
	}
} /* namespace nextar */
