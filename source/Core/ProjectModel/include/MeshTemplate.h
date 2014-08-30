
#ifndef MESHTEMPLATE_H_
#define MESHTEMPLATE_H_

#include <NexProjectModel.h>
#include <AssetTemplate.h>
#include <MeshBuffer.h>

namespace nextar {

class _NexProjectAPI MeshTemplate : 
	public AssetTemplate {
	NEX_LOG_HELPER(MeshTemplate);
public:
	enum Type {
		CLASS_ID = Component::CLASS_MESH_TEMPLATE,
		CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	class PrimitiveGroup : public AllocGeneral {
	public:

	protected:
		MaterialTemplatePtr material;
		MeshBuffer buffer;
	};

	typedef list<PrimitiveGroup> PrimitiveGroupList;
	class MeshBuilder : public AssetStreamRequest {
		
	};

protected:

	MaterialTemplatePtr sharedMaterial;
	MeshBuffer sharedBuffer;
	PrimitiveGroupList primitives;
};

}

#endif //MESHTEMPLATE_H_