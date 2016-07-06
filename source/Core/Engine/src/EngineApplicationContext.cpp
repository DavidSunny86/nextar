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
#include <AssetStreamer.h>

namespace nextar {


EngineApplicationContext::EngineApplicationContext() {
}

EngineApplicationContext::~EngineApplicationContext() {
}

void EngineApplicationContext::CreateExtendedInterfacesImpl() {
	NEX_NEW(TaskSchedular());
	NEX_NEW(InputManagerImpl());
	NEX_NEW(ComponentFactoryArchive());
	NEX_NEW(ComponentGroupArchive());
	NEX_NEW(MeshServices());
	NEX_NEW(AssetStreamer());
	ComponentFactoryArchive::Instance().InstallDefaultFactories();
}

void EngineApplicationContext::ConfigureExtendedInterfacesImpl(Config& config) {
	//TaskSchedular::Instance().Configure(GetConfig());
	InputManager::Instance().Configure(config);
	ComponentFactoryArchive::Instance().Configure(config);
	ComponentGroupArchive::Instance().Configure(config);
	if (RenderManager::InstancePtr()) {
		RenderManager::Instance().Configure(config);
	}
	MeshServices::Instance().Configure(config);
}

void EngineApplicationContext::ReleaseResources() {
	TaskSchedular::Instance().Close();
	MeshServices::Instance().Close();
	ComponentGroupArchive::Instance().AsyncDeleteAll();
	ComponentFactoryArchive::Instance().AsyncDeleteAll();
	if (RenderManager::InstancePtr()) {
		VertexLayout::ClearCommonLayouts();
		ApplicationContext::Instance().DispatchEvent(EVENT_RENDERMANAGER_PRE_CLOSE);
		RenderManager::Instance().Close();
		ApplicationContext::Instance().DispatchEvent(EVENT_RENDERMANAGER_POST_CLOSE);
	}
	InputManager::Instance().Close();
}

void EngineApplicationContext::DestroyExtendedInterfacesImpl() {
	NEX_DELETE(AssetStreamer::InstancePtr());
	NEX_DELETE(MeshServices::InstancePtr());
	NEX_DELETE(ComponentFactoryArchive::InstancePtr());
	NEX_DELETE(ComponentGroupArchive::InstancePtr());
	NEX_DELETE(InputManager::InstancePtr());
	NEX_DELETE(TaskSchedular::InstancePtr());
}

} /* namespace nextar */
