/*
 * Entity.cpp
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#include "Entity.h"
#include "Mesh.h"
#include "Light.h"
#include "Moveable.h"

namespace nextar {

	/*************************************
	 * EntityManager
	 *************************************/
	EntityManager::EntityManager(const String& name) :
			ComponentManagerImpl(name) {
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
		Moveable* lastMoveable = traversal.currentMoveable;

		if (moveable)
			traversal.currentMoveable = moveable;
		if (renderable)
			renderable->RegisterVisibles(traversal);
		if (light)
			light->RegisterVisibles(traversal);

		traversal.currentMoveable = lastMoveable;
	}

	int Entity::GetType() const {
		return Entity::TYPE;
	}

} /* namespace nextar */
