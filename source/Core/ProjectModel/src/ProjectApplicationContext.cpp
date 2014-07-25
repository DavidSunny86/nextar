/*
 * ProjectApplicationContext.cpp
 *
 *  Created on: 20-Jul-2014
 *      Author: obhi
 */
#include <NexProjectModel.h>
#include <ProjectApplicationContext.h>
#include <ShaderTemplate.h>

namespace nextar {

ProjectApplicationContext::ProjectApplicationContext(const String& name) :
	EngineApplicationContext(name) {
}

ProjectApplicationContext::~ProjectApplicationContext() {
}

void ProjectApplicationContext::CreateExtendedInterfacesImpl() {
	RegisterTemplateFactories();
}

void ProjectApplicationContext::RegisterTemplateFactories() {
	ShaderTemplate::FactoryTraits::_InternalRegisterToArchive();
}

void ProjectApplicationContext::ReleaseResources() {

}

} /* namespace nextar */
