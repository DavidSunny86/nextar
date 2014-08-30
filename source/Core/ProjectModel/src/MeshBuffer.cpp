
#include <NexProjectModel.h>
#include <MeshBuffer.h>

namespace nextar {


MeshBuffer::MeshBuffer(PrimitiveType _type) :
type(_type) {
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
			return channels.size() - 1;
		}
	}
	return -1;
}

}