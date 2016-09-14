
#include <ProjectContext.h>

namespace nextar {

ProjectContext::ProjectContext() : 
_projectName(StringUtils::Default),
_basePath(FileSystem::ArchiveProjectData) {

}

ProjectContext::ProjectContext(const String& name, const String& basePath) :
_projectName(name),
_basePath(basePath) {
	FileSystem::Instance().AddArchive(FileSystem::ArchiveProjectData_Name, Archive::TYPE_DIR, _basePath);
}

}