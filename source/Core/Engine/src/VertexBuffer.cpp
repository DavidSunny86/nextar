#include <NexEngine.h>
#include <VertexBuffer.h>

namespace nextar {

VertexBuffer::VertexBuffer(VertexBuffer&& other) :
GpuBuffer(std::move(other)), policy(other.policy) {
}

VertexBuffer::VertexBuffer(RelocationPolicy _policy) :
GpuBuffer(TYPE_VERTEX_BUFFER, policy), policy(_policy) {
}

void VertexBuffer::CreateBuffer(uint32 bufferSize, uint32 stride, const uint8* dataPtr) {
	GpuBuffer::CreateParams params;
	params.size = bufferSize;
	params.sourceData = dataPtr;
	params.policy = policy;
	params.elementStride = stride;
	RequestUpdate(MSG_CREATE,
				  static_cast<ContextObject::ContextParamPtr>(&params));
}

VertexBufferPtr VertexBuffer::Create(RelocationPolicy _policy) {
	VertexBufferPtr ret = Assign(NEX_NEW(VertexBuffer(_policy)));
	return ret;
}

}

