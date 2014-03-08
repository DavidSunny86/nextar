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
	 * Entity::Factory
	 *************************************/
	Entity::Factory::Factory(const StringID name) :
			Component::Factory(name) {
	}

	EntityPtr Entity::Factory::AsyncCreateCameraEntity(const StringID name) {
		return AsyncCreateAndAttach(name, Camera::CLASS_ID, name);
	}

	EntityPtr Entity::Factory::AsyncCreateMeshEntity(const StringID name, MeshAssetPtr mesh) {
		Component* _subComponent = nullptr;
		EntityPtr ret = AsyncCreateAndAttach(name, Mesh::CLASS_ID, name, &_subComponent);
		if (_subComponent) {
			Mesh* meshPtr = static_cast<Mesh*>(_subComponent);
			meshPtr->SetMeshAsset(mesh);
		}
		return ret;
	}

	EntityPtr Entity::Factory::AsyncCreateLightEntity(const StringID name) {
		return AsyncCreateAndAttach(name, Light::CLASS_ID, name);
	}

	EntityPtr Entity::Factory::AsyncCreateAndAttach(const StringID name, uint32 subType, const StringID subName, Component** _subComponent) {
		EntityPtr ent = Assign(AsyncCreate(Entity::CLASS_ID, name));
		Component* subComponent = AsyncCreate(subType, subName);
		if (subComponent) {
			ent->AttachComponent(subComponent);
			if (_subComponent) 
				*_subComponent = subComponent;
		} else {
			Warn("Failed to create subcomponent for entity: " + name);
		}
		return ent;
	}

	Component* Entity::Factory::AsyncCreate(uint32 type, const StringID name) {
		switch(type) {
		case Entity::CLASS_ID:
			return NEX_NEW Entity(name);
		case Light::CLASS_ID:
			return NEX_NEW Light(name);
		case Mesh::CLASS_ID:
			return NEX_NEW Mesh(name);
		case Moveable::CLASS_ID:
			return NEX_NEW Moveable(name);
		case Camera::CLASS_ID:
			return NEX_NEW Camera(name);
		}
		return 0;
	}

	/*************************************
	 * Entity
	 *************************************/
	Entity::Entity(const String& name) : 
		SharedComponent(name),
		moveable(nullptr),	spatial(nullptr), scene(nullptr) {
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
		/* todo Fire event */
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

	void Entity::RemoveFromScene(bool removeFromGroup) {
		if(scene) {
			scene->_RemoveEntity(this);
			scene = nullptr;
		}
		if (removeFromGroup) 
			RemoveFromGroup();
	}
} /* namespace nextar */
