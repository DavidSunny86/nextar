/*
 * Entity.cpp
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#include <Behaviour.h>
#include <NexEngine.h>
#include <Entity.h>
#include <Mesh.h>
#include <Light.h>
#include <Moveable.h>
#include <Camera.h>
#include <SceneAsset.h>
#include <MeshAsset.h>
#include <BVCullingSystem.h>

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

EntityPtr Entity::Factory::AsyncCreateMeshEntity(const StringID name,
		MeshAssetPtr mesh) {
	Component* _subComponent = nullptr;
	EntityPtr ret = AsyncCreateAndAttach(name, Mesh::CLASS_ID, name,
			&_subComponent);
	if (_subComponent) {
		Mesh* meshPtr = static_cast<Mesh*>(_subComponent);
		meshPtr->SetMeshAsset(mesh);
	}
	return ret;
}

EntityPtr Entity::Factory::AsyncCreateSkyLightEntity(const StringID name, const Color& color) {
	Component* _subComponent = nullptr;
	EntityPtr ret = AsyncCreateAndAttach(name, Light::CLASS_ID, name, &_subComponent);
	if (_subComponent) {
		Light* light = static_cast<Light*>(_subComponent);
		light->SetLightColor(color);
		light->SetLightType(Light::Type::SKY);
	}
	return ret;
}

EntityPtr Entity::Factory::AsyncCreateDirectionalLightEntity(const StringID name, const Color& color) {
	Component* _subComponent = nullptr;
	EntityPtr ret = AsyncCreateAndAttach(name, Light::CLASS_ID, name, &_subComponent);
	if (_subComponent) {
		Light* light = static_cast<Light*>(_subComponent);
		light->SetLightColor(color);
		light->SetLightType(Light::Type::DIRECTIONAL);
	}
	return ret;
}

EntityPtr Entity::Factory::AsyncCreateOmniLightEntity(const StringID name, const Color& color, float range) {
	Component* _subComponent = nullptr;
	EntityPtr ret = AsyncCreateAndAttach(name, Light::CLASS_ID, name, &_subComponent);
	if (_subComponent) {
		Light* light = static_cast<Light*>(_subComponent);
		light->SetLightColor(color);
		light->SetLightRange(range);
		light->SetLightType(Light::Type::OMNI);
	}
	return ret;
}

EntityPtr Entity::Factory::AsyncCreateAndAttach(const StringID name,
		uint32 subType, const StringID subName, Component** _subComponent) {
	EntityPtr ent = Assign(
			static_cast<Entity*>(AsyncCreate(Entity::CLASS_ID, name)));
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
	switch (type) {
	case Entity::CLASS_ID:
		return NEX_NEW(Entity(name, GetID()));
	case Light::CLASS_ID:
		return NEX_NEW(Light(name, GetID()));
	case Mesh::CLASS_ID:
		return NEX_NEW(Mesh(name, GetID()));
	case Moveable::CLASS_ID:
		return NEX_NEW(Moveable(name, GetID()));
	case Camera::CLASS_ID:
		return NEX_NEW(Camera(name, GetID()));
	case BVCullingSystem::CLASS_ID:
		return NEX_NEW(BVCullingSystem(name, GetID()));
	}
	return 0;
}

void Entity::Factory::_InternalRegisterToArchive() {
	Entity::Factory* entityFactory = NEX_NEW(Entity::Factory(StringUtils::DefaultID));
	ComponentFactoryArchive::Instance()._InternalDefaultFactory(entityFactory, Entity::CLASS_ID);
	ComponentFactoryArchive::Instance()._InternalDefaultFactory(entityFactory, Light::CLASS_ID);
	ComponentFactoryArchive::Instance()._InternalDefaultFactory(entityFactory, Mesh::CLASS_ID);
	ComponentFactoryArchive::Instance()._InternalDefaultFactory(entityFactory, Moveable::CLASS_ID);
	ComponentFactoryArchive::Instance()._InternalDefaultFactory(entityFactory, Camera::CLASS_ID);
	ComponentFactoryArchive::Instance()._InternalDefaultFactory(entityFactory, BVCullingSystem::CLASS_ID);
}

/*************************************
 * Entity
 *************************************/
Entity::Entity(const StringID name, const StringID factory) :
		SharedComponent(name, factory), moveable(nullptr),
		spatial(nullptr), scene(nullptr), behaviour(nullptr) {
}

Entity::~Entity() {
	if (moveable)
		NEX_DELETE(moveable);
	if (spatial)
		NEX_DELETE(spatial);
	if (behaviour)
		NEX_DELETE(behaviour);
}

void Entity::AttachComponent(Component* comp) {
	NEX_ASSERT (comp);
	uint32 type = comp->GetClassID();
	uint16 catagory = Component::GetComponentCatagory(type);

	if (catagory & Entity::CATAGORY) {
		Error("Sub-entities not supported yet!");
	}

	if (catagory & Spatial::CATAGORY) {
		if (spatial) {
			Error("Only a single spatial can be attached to an entity!");
			return;
		}
		spatial = static_cast<Spatial*>(comp);
		spatial->SetMoveable(moveable);
	}

	if (catagory & Moveable::CATAGORY) {
		moveable = static_cast<Moveable*>(comp);
		if (spatial)
			spatial->SetMoveable(moveable);
	}

	if (catagory & Behaviour::CATAGORY) {
		behaviour = static_cast<Behaviour*>(comp);
		// should add this to scene update
	}
	comp->SetParent(this);
	/* todo Fire event */
}

void Entity::DetachComponent(uint32 type) {
	Component* toDetach = nullptr;
	uint16 catagory = Component::GetComponentCatagory(type);
	if (catagory & Entity::CATAGORY) {
		Error("Sub-entities not supported yet!");
	}

	if (catagory & Spatial::CATAGORY) {
		NEX_ASSERT(!toDetach || toDetach == static_cast<Component*>(spatial));
		toDetach = spatial;
		spatial = nullptr;
	}

	if (catagory & Moveable::CATAGORY) {
		NEX_ASSERT(!toDetach || toDetach == static_cast<Component*>(moveable));
		toDetach = moveable;
		moveable = nullptr;
		if (spatial)
			spatial->SetMoveable(nullptr);
	}

	if (toDetach) {
		toDetach->SetParent(nullptr);
		NEX_DELETE(toDetach);
	} // else report error ??
}

uint32 Entity::GetClassID() const {
	return Entity::CLASS_ID;
}

void Entity::AddToScene(SceneAsset* scene) {
	this->scene = scene;
	scene->_AddEntity(this);
}

void Entity::RemoveFromScene(bool removeFromGroup) {
	if (scene) {
		scene->_RemoveEntity(this);
		scene = nullptr;
	}
	if (removeFromGroup)
		RemoveFromGroup();
}

void Entity::SetTransform(Vec3AF position, QuatF rotation, float scaling) {

	if (moveable) {
		moveable->SetTransform(position, rotation, scaling);
		if (spatial)
			spatial->SetUpdateRequired(true);
	} else if (spatial)
		spatial->SetTransform(position, rotation, scaling);
	SetUpdateRequired(true);
}

void Entity::Update(const FrameTimer& frameTimer) {
	if (behaviour)
		behaviour->Update(frameTimer);
	if (moveable && moveable->IsUpdateRequired())
		moveable->Update(frameTimer);
	if (spatial && spatial->IsUpdateRequired())
		spatial->Update(frameTimer);
	SetUpdateRequired(false);
}

} /* namespace nextar */
