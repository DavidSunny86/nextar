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
#include <ResourcePackerService.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(ResourcePacker);

ProjectApplicationContext::ProjectApplicationContext() :
	EngineApplicationContext()
	, _project(NEX_NEW(ProjectContext())) {
	
}

ProjectApplicationContext::~ProjectApplicationContext() {
}

void ProjectApplicationContext::CreateExtendedInterfacesImpl() {
	EngineApplicationContext::CreateExtendedInterfacesImpl();
	RegisterTemplateFactories();
}

void ProjectApplicationContext::CreateProjectContext(
		const String& projectName, const String& path) {
	_project.reset(NEX_NEW(ProjectContext(projectName, path)));
}

void ProjectApplicationContext::RegisterTemplateFactories() {
	ShaderTemplate::FactoryTraits::_InternalRegisterToArchive();
	MaterialTemplate::FactoryTraits::_InternalRegisterToArchive();
	MeshTemplate::FactoryTraits::_InternalRegisterToArchive();

	AssetSaver::RegisterFactory("NEXMTL",
			MaterialTemplate::Traits::CLASS_ID,
			&MaterialSaverImplv1_0::saver);
	AssetSaver::RegisterFactory("NEXFX",
			ShaderTemplate::Traits::CLASS_ID,
			&ShaderSaverImplv1_0::saver);
	AssetSaver::RegisterFactory("NEXMESH",
				MeshTemplate::Traits::CLASS_ID,
				&MeshSaverImplv1_0::saver);
}

void ProjectApplicationContext::ReleaseResources() {
	EngineApplicationContext::ReleaseResources();
}

} /* namespace nextar */
