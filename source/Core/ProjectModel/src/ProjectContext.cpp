
#include <ProjectContext.h>

namespace nextar {

ProjectContext::ProjectContext() :
_projectName(StringUtils::Default),
_basePath(FileSystem::ArchiveProjectData) {

	_resourceLocations[RESOURCE_MESH] = "Meshes";
	_resourceLocations[RESOURCE_TEXTURE] = "Textures";
	_resourceLocations[RESOURCE_MATERIAL] = "Materials";
	_resourceLocations[RESOURCE_RENDERSYS] = "Configs";
	_resourceLocations[RESOURCE_SHADER] = "Shaders";
	_resourceLocations[RESOURCE_SCRIPT_MATERIAL] = "Scripts/Materials";
	_resourceLocations[RESOURCE_SCRIPT_RENDERSYS] = "Scripts/Configs";
	_resourceLocations[RESOURCE_SCRIPT_SHADER] = "Scripts/Shaders";
}

ProjectContext::ProjectContext(const String& name, const String& basePath) :
_projectName(name),
_basePath(basePath) {
	FileSystem::Instance().AddArchive(FileSystem::ArchiveProjectData_Name, Archive::TYPE_DIR, _basePath);

}

ProjectContext::~ProjectContext() {
}

}
