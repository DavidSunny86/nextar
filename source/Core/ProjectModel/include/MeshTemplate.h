
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

	struct PrimitiveGroup : public AllocGeneral {

		PrimitiveGroup() : buffer(nullptr), startVertex(0),
				vertexCount(0), startIndex(0), indexCount(0) {}

		uint32 startVertex;
		uint32 vertexCount;
		uint32 startIndex;
		uint32 indexCount;
		MaterialTemplatePtr material;
		MeshBuffer* buffer;
	};

	typedef list<PrimitiveGroup>::type PrimitiveGroupList;
	class MeshBuilder : public AssetStreamRequest {
	public:

		PrimitiveGroup& AddPrimitiveGroup() {
			primitives.push_back(PrimitiveGroup());
			return primitives.back();
		}

		void SetSharedMaterial(const MaterialTemplatePtr& material) {
			sharedMaterial = material;
		}

		void SetSharedBuffer(MeshBuffer* buffer) {
			sharedBuffer = buffer;
		}
		
	protected:
		MaterialTemplatePtr sharedMaterial;
		PrimitiveGroupList primitives;
		MeshBuffer* sharedBuffer;
	};

protected:

	MaterialTemplatePtr sharedMaterial;
	MeshBuffer* sharedBuffer;
	PrimitiveGroupList primitives;
};

}

#endif //MESHTEMPLATE_H_
