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

namespace nextar {


EngineApplicationContext::EngineApplicationContext(const String& name) :
	ApplicationContext(name) {
}

EngineApplicationContext::~EngineApplicationContext() {
}

void EngineApplicationContext::CreateExtendedInterfacesImpl() {
	NEX_NEW(ComponentFactoryArchive());
	NEX_NEW(ComponentGroupArchive());
	NEX_NEW(MeshServices());
	ComponentFactoryArchive::Instance().InstallDefaultFactories();
}

void EngineApplicationContext::ConfigureExtendedInterfacesImpl() {
	ComponentFactoryArchive::Instance().Configure(GetConfig());
	ComponentGroupArchive::Instance().Configure(GetConfig());
	if (RenderManager::InstancePtr()) {
		RenderManager::Instance().Configure(GetConfig());
	}
	MeshServices::Instance().Configure(GetConfig());
}

void EngineApplicationContext::DestroyExtendedInterfacesImpl() {
	ReleaseResources();
	NEX_DELETE(MeshServices::InstancePtr());
	VertexLayout::ClearCommonLayouts();
	if (RenderManager::InstancePtr()) {
		DispatchEvent(EVENT_RENDERMANAGER_PRE_CLOSE);
		RenderManager::Instance().Close();
		DispatchEvent(EVENT_RENDERMANAGER_POST_CLOSE);
	}
	NEX_DELETE(ComponentFactoryArchive::InstancePtr());
	NEX_DELETE(ComponentGroupArchive::InstancePtr());
}

} /* namespace nextar */
