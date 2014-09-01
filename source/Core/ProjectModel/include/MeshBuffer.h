
#ifndef MESHBUFFER_H_
#define MESHBUFFER_H_

#include <NexProjectModel.h>
#include <AssetTemplate.h>

namespace nextar {


typedef nextar::vector<uint32>::type IndexArray;
typedef nextar::vector<Vector3>::type Vec3Array;
typedef nextar::vector<Vector4>::type Vec4Array;
typedef nextar::vector<Vector2>::type Vec2Array;
typedef nextar::vector<Vec3Array>::type Vec3ArrayList;
typedef nextar::vector<Vec2Array>::type Vec2ArrayList;

class VertexChannel : public AllocGeneral {
public:

	VertexChannel(VertexComponentSemantic _semantic, uint32 _semanticIdx, VertexComponentType _type) :
		semantic(_semantic), semanticIdx(_semanticIdx), type(_type) {
	}

	virtual ~VertexChannel() {
	}

	virtual void Reserve(uint32 count) = 0;
	virtual bool Equals(uint32 i, uint32 j) = 0;
	virtual void PushVertex(const void* pData) = 0;
	virtual void PushVertices(const void* pData, uint32 numVertices) = 0;
	virtual uint32 GetVertexCount() const = 0;

	inline VertexComponentSemantic GetSemantic() const {
		return semantic;
	}

	inline uint32 GetSemanticIndex() const {
		return semanticIdx;
	}

	inline VertexComponentType GetType() const {
		return type;
	}

protected:
	uint32 semanticIdx;
	VertexComponentType type;
	VertexComponentSemantic semantic;
};

typedef vector<VertexChannel*>::type VertexChannelList;

template <typename Vec>
class VertexChannelTempl : public VertexChannel {
public:
	typedef vector<Vec>::type VertexArray;

	VertexChannelTempl(VertexComponentSemantic _semantic, uint32 _index, VertexComponentType _type) :
		VertexChannel(_semantic, _index, _type) {
	}

	virtual void Reserve(uint32 count) {
		vertices.reserve(vertices.size() + count);
	}

	virtual bool Equals(uint32 i, uint32 j) {
		return Math::Traits<Vec>::Equals(vertices[i], vertices[j]);
	}

	virtual uint32 GetVertexCount() const {
		return vertices.size();
	}

	virtual void PushVertex(const void* pData) {
		const Vec& v = *reinterpret_cast<const Vec*>(pData);
		vertices.push_back(v);
	}

	virtual void PushVertices(const void* pData, uint32 numVertices) {
		const Vec* v = reinterpret_cast<const Vec*>(pData);
		for (uint32 i = 0; i < numVertices; ++i)
			vertices.push_back(v[i]);
	}

protected:
		
	VertexArray vertices;
};

class _NexProjectAPI MeshBuffer : public AllocGeneral {
public:

	
	MeshBuffer(PrimitiveType type);
	
	uint32 AddVertexChannel(
		VertexComponentSemantic _semantic, 
		uint32 _semanticIdx, 
		VertexComponentType _type);

	VertexChannel* GetVertexChannel(uint32 i);
	VertexChannel* GetVertexChannel(VertexComponentSemantic _semantic, uint32 _semanticIdx);
	uint32 GetChannelCount(VertexComponentSemantic _semantic);

	void RemoveDuplicates();

	void ReserveVertexSpace(uint32 nVertex);
	void ReserveIndexSpace(uint32 nIndex);
	uint32 GetVertexCount() const;
	uint32 GetIndexCount() const {
		return indices.size();
	}

	inline void PushIndex(uint32 index) {
		indices.push_back(index);
	}

	inline const String& GetVertexSignature() const {
		return vertexSignature;
	}

	void MergeBuffer(const MeshBuffer& m);

protected:

	String vertexSignature;
	PrimitiveType type;
	uint32 flags;
	IndexArray indices;
	VertexChannelList channels;
};


}
#endif //MESHBUFFER_H_
