/*
 * ProjectApplicationContext.cpp
 *
 *  Created on: 20-Jul-2014
 *      Author: obhi
 */
#include <NexProjectModel.h>
#include <ProjectApplicationContext.h>
#include <ShaderTemplate.h>
#include <ShaderSaverImplv10.h>
#include <MaterialSaverImplv10.h>
#include <ShaderTemplate.h>
#include <MaterialTemplate.h>

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
	MaterialTemplate::FactoryTraits::_InternalRegisterToArchive();

	AssetSaver::RegisterFactory("MTL",
			MaterialTemplate::Traits::CLASS_ID,
			&MaterialSaverImplv1_0::saver);
	AssetSaver::RegisterFactory("NFX",
			ShaderTemplate::Traits::CLASS_ID,
			&ShaderSaverImplv1_0::saver);
}

void ProjectApplicationContext::ReleaseResources() {

}

} /* namespace nextar */
