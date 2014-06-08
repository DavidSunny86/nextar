
#include <NexEngine.h>
#include <VertexBuffer.h>

namespace nextar {

	VertexBuffer::VertexBuffer(size_t bufferSize, uint32 accessFlags, RelocationPolicy p) : 
		GpuBuffer(TYPE_VERTEX_BUFFER, bufferSize, accessFlags, p), frameThreshold(0) {
	}
	
}
