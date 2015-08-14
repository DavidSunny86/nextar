
#include <NexProjectModel.h>
#include <MeshBuffer.h>

namespace nextar {

template <typename Vec>
class VertexChannelTempl : public VertexChannel {
public:
	typedef typename vector<Vec>::type VertexArray;


	VertexChannelTempl(VertexComponentSemantic _semantic, uint32 _index, VertexComponentType _type, uint32 streamIdx) :
		VertexChannel(_semantic, _index, _type, Math::Traits<Vec>::Size(), streamIdx) {
	}

	virtual VertexChannel* CreateEmpty() const {
		return NEX_NEW(VertexChannelTempl<Vec>(semantic, semanticIdx, type, streamIndex));
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
		return (uint32)vertices.size();
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
		VertexChannelTempl<Vec>* c = static_cast< VertexChannelTempl<Vec>* >(channel);
		vertices.insert(vertices.end(), c->vertices.begin(), c->vertices.end());
	}

	virtual const void* GetVertex(uint32 i) const {
		return &vertices[i];
	}

	virtual void GetVerties(void* destBuffer, uint32 outStride) {
		uint8* vert = static_cast<uint8*>(destBuffer);
		for(auto& e : vertices) {
			std::memcpy(vert, &e, stride);
			vert += outStride;
		}
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

VertexChannel* MeshBuffer::GetVertexChannel(uint32 i) const {
	return channels[i];
}

uint32 MeshBuffer::GetVertexBufferCount() const {
	uint32 vbCount = 0;
	uint32 currentStream = -1;
	for (auto &c : channels) {
		if (currentStream != c->GetStreamIndex()) {
			currentStream = c->GetStreamIndex();
			vbCount++;
		}
	}

	return vbCount;
}

uint32 MeshBuffer::GetVertexCount() const {
	if (channels.size())
		return channels[0]->GetVertexCount();
	return 0;
}

VertexChannel* MeshBuffer::GetVertexChannel(VertexComponentSemantic _semantic, uint32 _semanticIdx) const {
	for (auto &c : channels) {
		if (c->GetSemantic() == _semantic &&
			c->GetSemanticIndex() == _semanticIdx)
			return c;
	}

	return nullptr;
}

uint32 MeshBuffer::GetChannelCount(VertexComponentSemantic _semantic) const {
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
	VertexComponentType _type,
	uint32 streamIdx) {
	uint32 index = 0;
	for (auto &c : channels) {
		if (c->GetSemantic() == _semantic &&
			c->GetSemanticIndex() == _semanticIdx)
			return index;
		index++;
	}

	VertexChannel* channel = nullptr;
	switch (_type) {
	case COMP_TYPE_FLOAT1:
		channel = NEX_NEW(VertexChannelTempl<float>(_semantic, _semanticIdx, _type, streamIdx));
		break;
	case COMP_TYPE_FLOAT2:
		channel = NEX_NEW(VertexChannelTempl<Vector2>(_semantic, _semanticIdx, _type, streamIdx));
		break;
	case COMP_TYPE_FLOAT3:
		channel = NEX_NEW(VertexChannelTempl<Vector3>(_semantic, _semanticIdx, _type, streamIdx));
		break;
	case COMP_TYPE_FLOAT4:
		channel = NEX_NEW(VertexChannelTempl<Vector4>(_semantic, _semanticIdx, _type, streamIdx));
		break;
	case COMP_TYPE_UNSIGNED_INT:
	case COMP_TYPE_COLOR:
		channel = NEX_NEW(VertexChannelTempl<uint32>(_semantic, _semanticIdx, _type, streamIdx));
		break;
	}

	if (channel) {
		channels.push_back(channel);
		vertexSignature += "#";
		vertexSignature += Convert::ToString((uint32)_semantic) + ":" + Convert::ToString((uint32)_semanticIdx);
		return (uint32)channels.size() - 1;
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

BoundsInfo MeshBuffer::ComputeBounds() const {
	BoundsInfo bounds;
	VertexChannel* vc = GetVertexChannel(COMP_POSITION, 0);
	if (vc && vc->GetType() == COMP_TYPE_FLOAT3) {
		const Vector3* v = reinterpret_cast<const Vector3*>(vc->GetVertex(0));
		uint32 nCount = vc->GetVertexCount();
		Vector3 minPoint(Math::SCALAR_MAX), maxPoint(-Math::SCALAR_MAX);
		for(uint32 i = 0; i < nCount; ++i) {
			v[i].GetMinElements(minPoint);
			v[i].GetMaxElements(maxPoint);
		}

		bounds.center = (maxPoint + minPoint) * 0.5f;
		bounds.extends = (maxPoint - minPoint) * 0.5f;
		bounds.radius = bounds.extends.Length();
	}

	return bounds;
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

void MeshBuffer::GetVertex(uint32 i, uint32 stream, ByteStream& out) const {
	out.clear();
	for (auto& e : channels) {
		if (e->GetStreamIndex() == stream) {
			const uint8* data = reinterpret_cast<const uint8*>(e->GetVertex(i));
			size_t size = e->GetStride();
			out.insert(out.end(), data, data+size);
		}
	}
}

void MeshBuffer::GetVertices(uint32 stream, ByteStream& out) const {
	out.clear();
	uint32 stride = GetVertexStride(stream);
	uint32 totalVertSize = stride * GetVertexCount();
	out.resize(totalVertSize);
	uint8* buff = out.data();
	for (auto& e : channels) {
		if (e->GetStreamIndex() == stream) {
			e->GetVerties(buff, stride);
			buff += e->GetStride();
		}
	}
}

void MeshBuffer::GetOptimizedIndices(ByteStream& out) const {
	uint32 count = GetVertexCount();
	if (count > std::numeric_limits<uint16>::max()) {
		out.resize(sizeof(uint32)*indices.size());
		std::memcpy(out.data(), indices.data(), out.size());
	} else {
		out.resize(sizeof(uint16)*indices.size());
		uint16* data = reinterpret_cast<uint16*>(out.data());
		for(uint32 i = 0; i < indices.size(); ++i)
			data[i] = (uint16)indices[i];
	}
}

uint32 MeshBuffer::GetVertexStride(uint32 streamIdx) const {
	uint32 stride = 0;
	for (auto& e : channels) {
		if (e->GetStreamIndex() == streamIdx)
			stride += e->GetStride();
	}
	return stride;
}

void MeshBuffer::GetVertexLayout(VertexLayoutInfo& layout) const {
	// all elements go to a single buffer
	// elements are laid out as channels are laid out
	layout.layoutType = CUSTOM_LAYOUT;
	uint32 layoutCount = VERTEX_LAYOUT_COUNT;
	for(uint32 i = 0; i < layoutCount; ++i) {
		const VertexElement* elements = nullptr;
		uint32 numElements = 0;
		VertexLayout::GetCommonLayoutElements((VertexLayoutType)i, elements, numElements);
		if (numElements == channels.size()) {
			int32 found = (int32)i;
			for (uint32 j = 0; j < channels.size(); ++j) {
				if( !(channels[j]->GetSemantic() == elements[j].desc.semantic.semantic &&
					channels[j]->GetSemanticIndex() == elements[j].desc.semantic.semanticIndex &&
					channels[j]->GetType() == elements[j].desc.semantic.type &&
					channels[j]->GetStreamIndex() == elements[j].streamIndex) ) {
					found = -1;
					break;
				}
			}
			if (found >= 0) {
				layout.layoutType = (VertexLayoutType)i;
				layout.vertexElements.assign(elements, elements+numElements);
				return;
			}
		}
	}

	layout.vertexElements.reserve(channels.size());
	uint16 offset = 0;
	for (auto& e : channels) {

		layout.vertexElements.push_back(
			VertexElement(
				VertexDesc(
					VertexSemantic(
						e->GetSemantic(), e->GetSemanticIndex(), e->GetType()), offset), e->GetStreamIndex())
				);
		offset += e->GetStride();
	}
}

}
