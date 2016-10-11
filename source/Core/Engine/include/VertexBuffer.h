#ifndef NEXTAR_VERTEXBUFFER_H
#define NEXTAR_VERTEXBUFFER_H

#include <GpuBuffer.h>

namespace nextar {

class _NexEngineAPI VertexBuffer: public GpuBuffer {

public:

	VertexBuffer(const VertexBuffer& ) : policy(GpuBuffer::INVALID_POLICY) {}
	VertexBuffer() : policy(GpuBuffer::INVALID_POLICY) {}
	VertexBuffer(VertexBuffer&& other);
	VertexBuffer(RelocationPolicy _policy);

	static VertexBufferPtr Create(RelocationPolicy _policy);

	inline bool IsTransientBuffer() const {
		return (policy == GpuBuffer::RelocationPolicy::REGULARLY_RELEASED ||
				policy == GpuBuffer::RelocationPolicy::IMMEDIATELY_RELEASED);
	}

	virtual void CreateBuffer(uint32 bufferSize, uint32 stride, const uint8* dataPtr);

	inline VertexBuffer& operator = (VertexBuffer&& other) {
		GpuBuffer::operator =(std::move(other));
		policy = other.policy;
		return *this;
	}

protected:

	RelocationPolicy policy;
};

}

#endif //NEXTAR_VERTEXBUFFER_H