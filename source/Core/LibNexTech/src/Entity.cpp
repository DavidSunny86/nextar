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

	Component* EntityManager::AsyncCreateImpl(int type, const String& name) {
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
		if (light)
			NEX_DELETE light;
	}

	void Entity::FindVisibles(SceneTraversal& traversal) {
		Moveable* lastMoveable = traversal.moveable;

		if (moveable)
			traversal.moveable = moveable;
		if (renderable)
			renderable->RegisterVisibles(traversal);
		if (light)
			light->RegisterVisibles(traversal);

		traversal.moveable = lastMoveable;
	}

	void Entity::AttachComponent(Component* comp) {
		switch(comp->GetComponentCatagory()) {
		case Entity::CATAGORY:
			Error("Sub-entities not supported yet!");
		case Light::CATAGORY:
			light = static_cast<Light*>(comp);
			break;
		case Renderable::CATAGORY:
			renderable = static_cast<Renderable*>(comp);
			if (moveable)
				renderable->NotifyMoveable(moveable);
			break;
		case Camera::CATAGORY:
			SetFlag(IS_CAMERA);
			/* no break */
		case Moveable::CATAGORY:
			moveable = static_cast<Moveable*>(comp);
			if (renderable)
				renderable->NotifyMoveable(moveable);
			break;
		}
		comp->SetParent(this);
		/* @todo Fire event */
	}

	void Entity::DetachComponent(int type) {
		switch(type) {
		case Entity::CATAGORY:
			Error("Sub-entities not supported yet!");
		case Light::CATAGORY:
			if (light) {
				moveable->SetParent(nullptr);
				NEX_DELETE light;
			}
			light = nullptr;
			break;
		case Mesh::CATAGORY:
			if (renderable) {
				moveable->SetParent(nullptr);
				NEX_DELETE renderable;
			}
			renderable = nullptr;
			break;
		case Camera::CATAGORY:
			UnsetFlag(IS_CAMERA);
			/* no break */
		case Moveable::CATAGORY:
			if (moveable) {
				moveable->SetParent(nullptr);
				NEX_DELETE moveable;
				if (renderable)
					renderable->NotifyMoveable(moveable);
			}
			moveable = nullptr;
			break;
		}
	}

	int Entity::GetComponentType() const {
		return Entity::TYPE;
	}

	int Entity::GetComponentCatagory() const {
		return Entity::CATAGORY;
	}

} /* namespace nextar */
