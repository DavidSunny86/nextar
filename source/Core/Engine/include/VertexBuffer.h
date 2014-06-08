#ifndef NEXTAR_VERTEXBUFFER_H
#define NEXTAR_VERTEXBUFFER_H

#include <GpuBuffer.h>

namespace nextar {

	
	class _NexEngineAPI VertexBuffer: public GpuBuffer {

	public:

		VertexBuffer(size_t bufferSize, uint32 accessFlags, RelocationPolicy);

		void SetFrameThreshold(uint32 frameThreshold) {
			this->frameThreshold = frameThreshold;
		}

		uint32 GetFrameThreshold() const {
			return frameThreshold;
		}

	protected:
		/* Vertex buffer relocation policy */
		uint32 frameThreshold;
	};

}

#endif //NEXTAR_VERTEXBUFFER_H
