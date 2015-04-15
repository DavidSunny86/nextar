/*
 * EngineApplicationCOntext.cpp
 *
 *  Created on: 06-Jul-2014
 *      Author: obhi
 */
#include <NexEngine.h>
#include <EngineApplicationContext.h>
#include <ComponentFactoryArchive.h>
#include <ComponentGroupArchive.h>
#include <RenderManager.h>
#include <VertexLayout.h>
#include <MeshServices.h>
#include <InputManagerImpl.h>
#include <TaskSchedular.h>

namespace nextar {


EngineApplicationContext::EngineApplicationContext(const String& name) :
	ApplicationContext(name) {
}

EngineApplicationContext::~EngineApplicationContext() {
}

void EngineApplicationContext::CreateExtendedInterfacesImpl() {
	NEX_NEW(TaskSchedular());
	NEX_NEW(InputManagerImpl());
	NEX_NEW(ComponentFactoryArchive());
	NEX_NEW(ComponentGroupArchive());
	NEX_NEW(MeshServices());
	ComponentFactoryArchive::Instance().InstallDefaultFactories();
}

void EngineApplicationContext::ConfigureExtendedInterfacesImpl() {
	// TaskSchedular::Instance().Configure(GetConfig());
	InputManager::Instance().Configure(GetConfig());
	ComponentFactoryArchive::Instance().Configure(GetConfig());
	ComponentGroupArchive::Instance().Configure(GetConfig());
	if (RenderManager::InstancePtr()) {
		RenderManager::Instance().Configure(GetConfig());
	}
	MeshServices::Instance().Configure(GetConfig());
}

void EngineApplicationContext::ReleaseResourcesImpl() {
	MeshServices::Instance().Close();
	if (RenderManager::InstancePtr()) {
		DispatchEvent(EVENT_RENDERMANAGER_PRE_CLOSE);
		RenderManager::Instance().Close();
		DispatchEvent(EVENT_RENDERMANAGER_POST_CLOSE);
	}
	ComponentGroupArchive::Instance().AsyncDeleteAll();
	ComponentFactoryArchive::Instance().AsyncDeleteAll();
	InputManager::Instance().Close();
	VertexLayout::ClearCommonLayouts();
	TaskSchedular::Instance().Close();
}

void EngineApplicationContext::DestroyExtendedInterfacesImpl() {
	NEX_DELETE(MeshServices::InstancePtr());
	NEX_DELETE(ComponentFactoryArchive::InstancePtr());
	NEX_DELETE(ComponentGroupArchive::InstancePtr());
	NEX_DELETE(InputManager::InstancePtr());
	NEX_DELETE(TaskSchedular::InstancePtr());
}

} /* namespace nextar */
