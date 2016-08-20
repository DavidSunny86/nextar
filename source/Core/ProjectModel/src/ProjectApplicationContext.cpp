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
#include <MeshTemplate.h>
#include <MaterialSaverImplv10.h>
#include <MeshSaverImplv10.h>

namespace nextar {

ProjectApplicationContext::ProjectApplicationContext() :
	EngineApplicationContext() {
}

ProjectApplicationContext::~ProjectApplicationContext() {
}

void ProjectApplicationContext::CreateExtendedInterfacesImpl() {
	EngineApplicationContext::CreateExtendedInterfacesImpl();
	RegisterTemplateFactories();
}

void ProjectApplicationContext::RegisterTemplateFactories() {
	ShaderTemplate::FactoryTraits::_InternalRegisterToArchive();
	MaterialTemplate::FactoryTraits::_InternalRegisterToArchive();
	MeshTemplate::FactoryTraits::_InternalRegisterToArchive();

	AssetSaver::RegisterFactory("nexmtl",
			MaterialTemplate::Traits::CLASS_ID,
			&MaterialSaverImplv1_0::saver);
	AssetSaver::RegisterFactory("nexfx",
			ShaderTemplate::Traits::CLASS_ID,
			&ShaderSaverImplv1_0::saver);
	AssetSaver::RegisterFactory("nexmesh",
				MeshTemplate::Traits::CLASS_ID,
				&MeshSaverImplv1_0::saver);
}

void ProjectApplicationContext::ReleaseResources() {
	EngineApplicationContext::ReleaseResources();
}

} /* namespace nextar */
