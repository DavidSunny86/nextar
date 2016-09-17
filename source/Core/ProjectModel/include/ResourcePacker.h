
#ifndef RESOURCEPACKER_H_
#define RESOURCEPACKER_H_

#include <NexBase.h>

namespace nextar {

class _NexProjectAPI ResourcePacker :
	public AllocGeneral {
public:

	virtual ~ResourcePacker() {}
	virtual void BuildMaterials(ProjectContextPtr const &context) = 0;
	virtual void BuildRenderScripts(ProjectContextPtr const &context) = 0;
	virtual void BuildShaders(ProjectContextPtr const &context) = 0;
	virtual void BuildProject(ProjectContextPtr const &context) = 0;
};

}

#endif //RESOURCEPACKER_H_
