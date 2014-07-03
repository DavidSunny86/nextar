/*
 * FactoryRoot.cpp
 *
 *  Created on: Oct 6, 2011
 *      Author: obhi
 */
#include <NexEngine.h>
#include <ComponentFactoryArchive.h>
#include <Entity.h>
#include <StringUtils.h>
#include <Light.h>
#include <Mesh.h>
#include <Moveable.h>
#include <Camera.h>
#include <SceneAsset.h>
#include <MaterialAsset.h>
#include <MeshAsset.h>
#include <ShaderAsset.h>
#include <TextureAsset.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(ComponentFactoryArchive);ComponentFactoryArchive ::ComponentFactoryArchive() {
}

ComponentFactoryArchive::~ComponentFactoryArchive() {
	AsyncDeleteAll();
}

void ComponentFactoryArchive::AsyncAddFactory(uint32 type,
		Component::Factory* factory) {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	componentFactories[FactoryID(factory->GetID(), type)] = factory;
}

void ComponentFactoryArchive::AsyncRemoveFactory(uint32 type, StringID name,
		bool d) {
	if (type == Component::CLASS_UNKNOWN) {
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		for (auto p = componentFactories.cbegin();
				p != componentFactories.cend();) {
			if ((*p).first.first == name) {
				if (d)
					NEX_DELETE((*p).second);
				componentFactories.erase(p++);
			} else {
				++p;
			}
		}
	} else {
		NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
		auto it = componentFactories.find(FactoryID(name, type));
		if (it != componentFactories.end()) {
			if (d)
				NEX_DELETE((*it).second);
			componentFactories.erase(it);
		}
	}
}

Component::Factory* ComponentFactoryArchive::AsyncFindFactory(uint32 type,
		const StringID name) {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	ComponentFactoryMap::iterator it = componentFactories.find(
			FactoryID(name, type));
	if (it != componentFactories.end())
		return (*it).second;
	return 0;
}

void ComponentFactoryArchive::AsyncDeleteAll() {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	// @optimize bubble search here
	for (auto it = componentFactories.begin(); it != componentFactories.end();) {
		if ((*it).second) {
			auto current = (*it).second;
			NEX_DELETE(current);
			it = componentFactories.erase(it);
			auto cur = it;
			while(cur != componentFactories.end()) {
				if ((*cur).second == current)
					(*cur).second = nullptr;
				cur++;
			}
		} else
			it = componentFactories.erase(it);
	}
	componentFactories.clear();
}

void ComponentFactoryArchive::Configure(const Config& config) {
	auto it = componentFactories.begin();
	auto en = componentFactories.end();
	StringID current = StringUtils::NullID;
	for (; it != en; ++it) {
		if ((*it).first.second != current) {	
			current = (*it).first.second;
			if((*it).second)
				(*it).second->Configure(config);
		}
	}
}

void ComponentFactoryArchive::InstallDefaultFactories() {
	NEX_THREAD_LOCK_GUARD_MUTEX(containerLock);
	// Assets
	SceneAsset::FactoryTraits::_InternalRegisterToArchive();
	MaterialAsset::FactoryTraits::_InternalRegisterToArchive();
	MeshAsset::FactoryTraits::_InternalRegisterToArchive();
	TextureAsset::FactoryTraits::_InternalRegisterToArchive();
	ShaderAsset::FactoryTraits::_InternalRegisterToArchive();
	// Entity factory
	Entity::Factory::_InternalRegisterToArchive();
}

} /* namespace nextar */
