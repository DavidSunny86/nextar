#ifndef NEXTAR_VERTEXBUFFER_H
#define NEXTAR_VERTEXBUFFER_H

#include <GpuBuffer.h>

namespace nextar {

	enum class RelocationPolicy {
		NEVER_RELEASE,
		RELEASE_IMMEDIATELY,
		RELEASE_ON_FRAME_THRESHOLD,
	};

	class _NexEngineAPI VertexBuffer: public GpuBuffer {

	public:

		VertexBuffer(size_t bufferSize, uint32 accessFlags, RelocationPolicy);

		void SetFrameThreshold(uint32 frameThreshold) {
			this->frameThreshold = frameThreshold;
		}

		uint32 GetFrameThreshold() const {
			return frameThreshold;
		}

		virtual void SetPolicy(RelocationPolicy policy) {
			this->policy = policy;
		}

		RelocationPolicy GetPolicy() const {
			return policy;
		}

	protected:
		/* Vertex buffer relocation policy */
		uint32 frameThreshold;
		RelocationPolicy policy;
	};

}

#endif //NEXTAR_VERTEXBUFFER_H
