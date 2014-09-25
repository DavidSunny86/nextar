
#include <NexProjectModel.h>
#include <MeshBuffer.h>

namespace nextar {

template <typename Vec>
class VertexChannelTempl : public VertexChannel {
public:
	typedef vector<Vec>::type VertexArray;


	VertexChannelTempl(VertexComponentSemantic _semantic, uint32 _index, VertexComponentType _type) :
		VertexChannel(_semantic, _index, _type) {
	}

	virtual VertexChannel* CreateEmpty() const {
		return NEX_NEW(VertexChannelTempl<Vec>());
	}

	virtual uint32 Hash(uint32 i) const {
		return Math::Traits<Vec>::Hash(vertices[i]);
	}

	virtual void Reserve(uint32 count) {
		vertices.reserve(vertices.size() + count);
	}

	virtual bool Equals(uint32 i, uint32 j) const {
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
		vertices.reserve(vertices.size() + numVertices);
		vertices.insert(vertices.end(), v, v+numVertices);
	}

	virtual void PushVertices(VertexChannel* channel) {
		VertexChannelTempl<Vec>* c = static_cast< VertexChannelTempl<Vec> >(channel);
		vertices.insert(vertices.end(), c->vertices.begin(), c->vertices.end());
	}

	virtual const void* GetVertex(uint32 i) const {
		return &vertices[i];
	}

protected:

	VertexArray vertices;
};


MeshBuffer::MeshBuffer(PrimitiveType _type) :
type(_type)
,flags(0) {
}

MeshBuffer::~MeshBuffer() {
	for (auto& e : channels) {
		NEX_DELETE(e);
	}
}

void MeshBuffer::ReserveVertexSpace(uint32 numVertices) {
	for (auto& e : channels) {
		e->Reserve(numVertices);
	}
}

void MeshBuffer::ReserveIndexSpace(uint32 indexCount) {
	indices.reserve(indices.size() + indexCount);
}

VertexChannel* MeshBuffer::GetVertexChannel(uint32 i) {
	return channels[i];
}

uint32 MeshBuffer::GetVertexCount() const {
	if (channels.size())
		return channels[0]->GetVertexCount();
	return 0;
}

VertexChannel* MeshBuffer::GetVertexChannel(VertexComponentSemantic _semantic, uint32 _semanticIdx) {
	for (auto &c : channels) {
		if (c->GetSemantic() == _semantic &&
			c->GetSemanticIndex() == _semanticIdx)
			return c;
	}

	return nullptr;
}

uint32 MeshBuffer::GetChannelCount(VertexComponentSemantic _semantic) {
	uint32 count = 0;
	for (auto &c : channels) {
		if (c->GetSemantic() == _semantic)
			count++;
	}

	return count;
}

uint32 MeshBuffer::AddVertexChannel(
	VertexComponentSemantic _semantic,
	uint32 _semanticIdx,
	VertexComponentType _type) {
	VertexChannel* channel = GetVertexChannel(_semantic, _semanticIdx);
	if (!channel) {
		switch (_type) {
		case COMP_TYPE_FLOAT1:
			channel = NEX_NEW(VertexChannelTempl<float>(_semantic, _semanticIdx, _type));
			break;
		case COMP_TYPE_FLOAT2:
			channel = NEX_NEW(VertexChannelTempl<Vector2>(_semantic, _semanticIdx, _type));
			break;
		case COMP_TYPE_FLOAT3:
			channel = NEX_NEW(VertexChannelTempl<Vector3>(_semantic, _semanticIdx, _type));
			break;
		case COMP_TYPE_FLOAT4:
			channel = NEX_NEW(VertexChannelTempl<Vector4>(_semantic, _semanticIdx, _type));
			break;
		case COMP_TYPE_UNSIGNED_INT:
		case COMP_TYPE_COLOR:
			channel = NEX_NEW(VertexChannelTempl<uint32>(_semantic, _semanticIdx, _type));
			break;
		}
		if (channel) {
			channels.push_back(channel);
			vertexSignature += "#";
			vertexSignature += Convert::ToString((uint32)_semantic) + ":" + Convert::ToString((uint32)_semanticIdx);
			return channels.size() - 1;
		}
	}

	return -1;
}

bool MeshBuffer::VertexEquals(uint32 i, uint32 j) {
	for (auto &c : channels) {
		if(!c->Equals(i, j))
			return false;
	}
	return true;
}

void MeshBuffer::RemoveDuplicates() {
	uint32 nCount = GetVertexCount();
	IndexArray remapped;

	remapped.reserve(nCount);
	for (uint32 i = 0; i < nCount; ++i)
		remapped.push_back(i);

	unordered_multimap<uint32_t, uint32_t>::type vertexMap;
	for (uint32_t i = 0; i < nCount; ++i) {
		uint32_t hashv = 0;
		for (auto &c : channels) {
			hashv += c->Hash(i);
		}
		vertexMap.emplace(hashv, i);
	}

	for (auto it = vertexMap.begin(), en = vertexMap.end(); it != en; ) {
		auto nex = it;
		do {
			nex++;
		} while (nex != en && nex->first == it->first);
		for (auto i = it; i != nex; ++i) {
			if (remapped[i->second] == i->second) {
				auto j = i;
				for (++j; j != nex; ++j) {
					if (VertexEquals(i->second, j->second)) {
						remapped[std::max(i->second, j->second)] = std::min(i->second, j->second);
					}
				}
			}
		}

		it = nex;
	}

	VertexChannelList dupChannel;
	dupChannel.reserve(channels.size());
	for (auto c : channels) {
		VertexChannel* vc = c->CreateEmpty();
		dupChannel.push_back(vc);
		vc->Reserve(nCount);
	}

	uint32_t vertexCount = 0;
	for (uint32_t i = 0; i < nCount; ++i) {
		if (remapped[i] == i) {
			remapped[i] = vertexCount++;
			for (uint32 c = 0; c < dupChannel.size(); ++c) {
				dupChannel[c]->PushVertex(channels[c]->GetVertex(i));
			}
		} else {
			remapped[i] = remapped[remapped[i]];
		}
	}

	Trace("Vertices removed: " + Convert::ToString(nCount - vertexCount));
	for (uint32_t i = 0; i < indices.size(); ++i)
		indices[i] = remapped[indices[i]];

	for (auto& e : channels) {
		NEX_DELETE(e);
	}

	channels = std::move(dupChannel);
}

void MeshBuffer::MergeBuffer(const MeshBuffer& m) {
	NEX_ASSERT(vertexSignature == m.vertexSignature);
	NEX_ASSERT(type == m.type);
	flags |= m.flags;
	indices.reserve(indices.size() + m.indices.size());
	indices.insert(indices.end(), m.indices.begin(), m.indices.end());

	for(uint32 c = 0; c < m.channels.size(); ++c) {
		channels[c]->PushVertices(m.channels[c]);
	}
}

}
