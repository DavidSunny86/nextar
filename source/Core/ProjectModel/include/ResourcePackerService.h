
#ifndef RESOURCEPACKER_H_
#define RESOURCEPACKER_H_

#include <BaseHeaders.h>

namespace nextar {

class _NexProjectAPI ResourcePackerService :
	public PluginService,
	public AllocGeneral {
public:

	NEX_DECLARE_SERVICE_INTERFACE(ResourcePackerService);

public:

	virtual ~ResourcePackerService() {}
	virtual void BuildMaterials(ProjectContextPtr const &context) = 0;
	virtual void BuildRenderScripts(ProjectContextPtr const &context) = 0;
	virtual void BuildProject(ProjectContextPtr const &context) = 0;
	virtual void Destroy() override = 0;
};

}

#endif //RESOURCEPACKER_H_
