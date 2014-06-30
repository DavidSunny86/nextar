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

namespace nextar {


EngineApplicationContext::EngineApplicationContext(const String& name) :
	ApplicationContext(name) {
}

EngineApplicationContext::~EngineApplicationContext() {
}

void EngineApplicationContext::CreateExtendedInterfacesImpl() {
	NEX_NEW(ComponentFactoryArchive());
	NEX_NEW(ComponentGroupArchive());
}

void EngineApplicationContext::ConfigureExtendedInterfacesImpl() {
	ComponentFactoryArchive::Instance().Configure(GetConfig());
	ComponentGroupArchive::Instance().Configure(GetConfig());
	if (RenderManager::InstancePtr()) {
		RenderManager::Instance().Configure(GetConfig());
	}
}

void EngineApplicationContext::DestroyExtendedInterfacesImpl() {
	NEX_DELETE(ComponentFactoryArchive::InstancePtr());
	NEX_DELETE(ComponentGroupArchive::InstancePtr());
}

} /* namespace nextar */
