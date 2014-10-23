
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

	struct _NexProjectAPI PrimitiveGroup : public AllocGeneral {

		inline PrimitiveGroup() : buffer(nullptr), startVertex(0),
				vertexCount(0), startIndex(0), indexCount(0) {}
		~PrimitiveGroup();

		uint32 startVertex;
		uint32 vertexCount;
		uint32 startIndex;
		uint32 indexCount;
		MaterialTemplatePtr material;
		BoundsInfo bounds;
		// this is null if shared buffer is being used
		MeshBuffer* buffer;
	};

	typedef vector<PrimitiveGroup>::type PrimitiveGroupList;
	class _NexProjectAPI MeshBuilder : public AssetStreamRequest {
	public:

		MeshBuilder(MeshTemplate* asset) : AssetStreamRequest(asset), sharedBuffer(nullptr) {
		}

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
		
		void SetBounds(const BoundsInfo& bounds) {
			this->bounds = bounds;
		}

	protected:
		friend class MeshTemplate;

		BoundsInfo bounds;
		MaterialTemplatePtr sharedMaterial;
		PrimitiveGroupList primitives;
		MeshBuffer* sharedBuffer;
	};

	typedef AssetTraits<MeshTemplate> Traits;
	typedef FactoryTraits<MeshTemplate> FactoryTraits;

	MeshTemplate(const StringID name, const StringID factory);
	~MeshTemplate();

	uint32 GetNumPrimitiveGroups() const {
		return (uint32)primitives.size();
	}

	void SetBounds(const BoundsInfo& bounds) {
		this->bounds = bounds;
	}

	inline const BoundsInfo& GetBounds() const {
		return bounds;
	}

	inline MeshBuffer* GetSharedMeshBuffer() const {
		return sharedBuffer;
	}

	inline MaterialTemplatePtr GetSharedMaterial() const {
		return sharedMaterial;
	}

	inline const PrimitiveGroup& GetPrimitive(uint32 i) const {
		return primitives[i];
	}

	virtual StreamNotification NotifyAssetLoadedImpl(StreamRequest* request);

	virtual uint32 GetClassID() const;


protected:

	virtual StreamRequest* CreateStreamRequestImpl(bool load);
	virtual void UnloadImpl();

	BoundsInfo bounds;
	MaterialTemplatePtr sharedMaterial;
	MeshBuffer* sharedBuffer;
	PrimitiveGroupList primitives;
};

}

#endif //MESHTEMPLATE_H_
