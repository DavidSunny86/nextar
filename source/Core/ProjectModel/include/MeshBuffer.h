
#ifndef MESHBUFFER_H_
#define MESHBUFFER_H_

#include <NexProjectModel.h>
#include <AssetTemplate.h>
#include <EngineHeaders.h>

namespace nextar {


typedef nextar::vector<uint32>::type IndexArray;
typedef nextar::vector<Vector3>::type Vec3Array;
typedef nextar::vector<Vector4>::type Vec4Array;
typedef nextar::vector<Vector2>::type Vec2Array;
typedef nextar::vector<Vec3Array>::type Vec3ArrayList;
typedef nextar::vector<Vec2Array>::type Vec2ArrayList;

class _NexProjectAPI VertexChannel : public AllocGeneral {
public:

	VertexChannel(VertexComponentSemantic _semantic, uint32 _semanticIdx, VertexComponentType _type, uint32 _stride,
			uint32 _stream) :
		semantic(_semantic), semanticIdx(_semanticIdx), type(_type), stride(_stride),
		streamIndex(_stream) {
	}

	virtual ~VertexChannel() {
	}

	virtual VertexChannel* CreateEmpty() const = 0;
	virtual void Reserve(uint32 count) = 0;
	virtual bool Equals(uint32 i, uint32 j) const = 0;
	virtual uint32 Hash(uint32 i) const = 0;
	virtual void PushVertex(const void* pData) = 0;
	virtual void PushVertices(const void* pData, uint32 numVertices) = 0;
	virtual void PushVertices(VertexChannel* channel) = 0;
	virtual uint32 GetVertexCount() const = 0;
	virtual const void* GetVertex(uint32 i) const = 0;
	virtual void GetVerties(void* destBuffer, uint32 outStride) = 0;

	inline VertexComponentSemantic GetSemantic() const {
		return semantic;
	}

	inline uint32 GetSemanticIndex() const {
		return semanticIdx;
	}

	inline VertexComponentType GetType() const {
		return type;
	}

	inline uint32 GetStride() const {
		return stride;
	}

	inline uint32 GetStreamIndex() const {
		return streamIndex;
	}

protected:
	uint32 streamIndex;
	uint32 stride;
	uint32 semanticIdx;
	VertexComponentType type;
	VertexComponentSemantic semantic;
};

typedef vector<VertexChannel*>::type VertexChannelList;

struct VertexLayoutInfo {
	VertexLayoutType layoutType;
	VertexElementList vertexElements;
};

class _NexProjectAPI MeshBuffer : public AllocGeneral {
	NEX_LOG_HELPER(MeshBuffer);

public:

	
	MeshBuffer(PrimitiveType type);
	~MeshBuffer();
	
	uint32 AddVertexChannel(
		VertexComponentSemantic _semantic, 
		uint32 _semanticIdx, 
		VertexComponentType _type,
		uint32 streamIndex = 0);

	VertexChannel* GetVertexChannel(uint32 i) const;
	VertexChannel* GetVertexChannel(VertexComponentSemantic _semantic, uint32 _semanticIdx) const;
	uint32 GetChannelCount(VertexComponentSemantic _semantic) const ;

	void RemoveDuplicates();

	BoundsInfo ComputeBounds() const;
	void ReserveVertexSpace(uint32 nVertex);
	void ReserveIndexSpace(uint32 nIndex);

	void GetVertexLayout(VertexLayoutInfo&) const;
	uint32 GetVertexCount() const;
	uint32 GetVertexBufferCount() const;
	uint32 GetIndexCount() const {
		return (uint32)indices.size();
	}

	uint32 GetVertexStride(uint32 streamIdx) const;

	inline void PushIndex(uint32 index) {
		indices.push_back(index);
	}

	inline void PushIndices(const uint32 indices[3]) {
		this->indices.push_back(indices[0]);
		this->indices.push_back(indices[1]);
		this->indices.push_back(indices[2]);
	}

	inline const String& GetVertexSignature() const {
		return vertexSignature;
	}

	inline PrimitiveType GetPrimitiveType() const {
		return type;
	}

	bool VertexEquals(uint32 i, uint32 j);
	void MergeBuffer(const MeshBuffer& m);

	void GetVertex(uint32 i, uint32 streamIdx, ByteStream& out) const;
	void GetVertices(uint32 streamIdx, ByteStream& out) const;
	void GetOptimizedIndices(ByteStream& out) const;

protected:

	String vertexSignature;
	PrimitiveType type;
	uint32 flags;
	IndexArray indices;
	VertexChannelList channels;
};


}
#endif //MESHBUFFER_H_
