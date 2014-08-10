#include <NexEngine.h>
#include <IndexBuffer.h>

namespace nextar {

IndexBuffer::IndexBuffer(IndexBuffer&& other) :
	GpuBuffer(std::move(other)), indexType(other.indexType), policy(other.policy) {
}

void IndexBuffer::CreateBuffer(size_t bufferSize,
		IndexBuffer::Type type, const uint8* dataPtr) {
	GpuBuffer::CreateParams params;
	this->indexType = type;
	params.size = bufferSize;
	params.sourceData = dataPtr;
	params.policy = policy;
	params.elementStride = type == TYPE_16BIT ? 2: 4;
	RequestUpdate(MSG_CREATE,
			static_cast<ContextObject::ContextParamPtr>(&params));
}

}

