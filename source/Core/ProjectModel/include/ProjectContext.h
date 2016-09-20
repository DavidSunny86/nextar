
#ifndef PROJECTCONTEXT_H_
#define PROJECTCONTEXT_H_

#include <NexProjectModel.h>
#include <ProjectResourceType.h>

namespace nextar {

class _NexProjectAPI ProjectContext : public AllocGeneral {
public:

	ProjectContext();
	ProjectContext(const String& name, const String& basePath);
	~ProjectContext();
	
	void SetResourcePath(ProjectResourceType type, const String& path) {
		_resourceLocations[type] = path;
	}

	String GetResourceFullPath(ProjectResourceType type) const {
		return URL::GetAppendedPath(_basePath, _resourceLocations[type]);
	}

	const String& GetResourcePath(ProjectResourceType type) const {
		return _resourceLocations[type];
	}


protected:

	typedef array<String, RESOURCE_TYPE_COUNT>::type PathList;

	PathList _resourceLocations;
	String _basePath;
	String _projectName;
};

typedef UniquePtr<ProjectContext>::type ProjectContextPtr;

}

#endif //PROJECTCONTEXT_H_
