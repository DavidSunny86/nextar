
#include <NexEngine.h>
#include <VertexBuffer.h>

namespace nextar {

	VertexBuffer::VertexBuffer(size_t bufferSize, uint32 accessFlags, RelocationPolicy p) : 
		ManagedBuffer(bufferSize, accessFlags), policy(p), frameThreshold(0) {
	}
	
}
