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
		return AsyncCreateAndAttach(name, Camera::CLASS_ID, name+"#Cam");
	}

	EntityPtr EntityManager::AsyncCreateMeshEntity(const String& name, MeshAssetPtr mesh) {
		Component* _subComponent = nullptr;
		EntityPtr ret = AsyncCreateAndAttach(name, Mesh::CLASS_ID, name+"#Mesh", &_subComponent);
		if (_subComponent) {
			Mesh* meshPtr = static_cast<Mesh*>(_subComponent);
			meshPtr->SetMeshAsset(mesh);
		}
	}

	EntityPtr EntityManager::AsyncCreateLightEntity(const String& name) {
		return AsyncCreateAndAttach(Light::CLASS_ID, name);
	}

	EntityPtr EntityManager::AsyncCreateAndAttach(const String& name, uint32 subType, const String& subName, Component** _subComponent) {
		EntityPtr ent = Assign(AsyncFindOrCreate(Entity::CLASS_ID, name));
		Component* subComponent = AsyncFindOrCreate(subType, subName);
		if (subComponent) {
			ent->AttachComponent(subComponent);
			if (_subComponent) 
				*_subComponent = subComponent;
		} else {
			Warn("Failed to create subcomponent for entity: " + name);
		}
		return ent;
	}

	Component* EntityManager::AsyncCreateImpl(uint32 type, const String& name) {
		switch(type) {
		case Entity::CLASS_ID:
			return NEX_NEW Entity(this, name);
		case Light::CLASS_ID:
			return NEX_NEW Light(this, name);
		case Mesh::CLASS_ID:
			return NEX_NEW Mesh(this, name);
		case Moveable::CLASS_ID:
			return NEX_NEW Moveable(this, name);
		case Camera::CLASS_ID:
			return NEX_NEW Camera(this, name);
		}
		return 0;
	}

	/*************************************
	 * Entity
	 *************************************/
	Entity::Entity(ComponentManager* creator, const String& name) : 
		moveable(nullptr),	spatial(nullptr) {
		SetCreator(creator);
		SetName(name);
	}

	Entity::~Entity() {
		if (moveable)
			NEX_DELETE moveable;
		if (spatial)
			NEX_DELETE spatial;
	}

	void Entity::AttachComponent(Component* comp) {
		uint32 type = comp->GetClassID();
		uint16 catagory = Component::GetComponentCatagory(type);

		if(catagory & Entity::CATAGORY) {
			Error("Sub-entities not supported yet!");
		}

		if(catagory & Spatial::CATAGORY) {
			if (spatial) {
				Error("Only a single spatial can be attached to an entity!");
				return;
			}
			spatial = static_cast<Spatial*>(comp);
			spatial->SetMoveable(moveable);
		}
			
		if(catagory & Moveable::CATAGORY) {
			moveable = static_cast<Moveable*>(comp);
			if (spatial)
				spatial->SetMoveable(moveable);
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

		if(catagory & Spatial::CATAGORY) {
			NEX_ASSERT(!toDetach || toDetach == static_cast<Component*>(spatial));
			toDetach = spatial;
			spatial = nullptr;
		}

		if(catagory & Moveable::CATAGORY) {
			NEX_ASSERT(!toDetach || toDetach == static_cast<Component*>(moveable));
			toDetach = moveable;
			moveable = nullptr;
			if (spatial)
				spatial->SetMoveable(nullptr);
		}

		if (toDetach) {
			toDetach->SetParent(nullptr);
			NEX_DELETE toDetach;
		} // else report error ??
	}

	uint32 Entity::GetClassID() const {
		return Entity::CLASS_ID;
	}

	void Entity::AddToScene(Scene* scene) {
		this->scene = scene;
		scene->_AddEntity(this);
	}

	void Entity::RemoveFromScene(bool removeFromCreator) {
		if(scene) {
			scene->_RemoveEntity(this);
			scene = nullptr;
		}
		if (removeFromCreator)
			creator->AsyncDestroy(this);
	}
} /* namespace nextar */
