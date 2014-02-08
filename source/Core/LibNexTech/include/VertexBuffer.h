#ifndef NEXTAR_VERTEXBUFFER_H
#define NEXTAR_VERTEXBUFFER_H

#include "ManagedBuffer.h"

namespace nextar {

	enum class RelocationPolicy {
		NEVER_RELEASE,
		RELEASE_IMMEDIATELY,
		RELEASE_ON_FRAME_THRESHOLD,
	};

	class _NexExport VertexBuffer: public ManagedBuffer {
	protected:
		/* Vertex buffer relocation policy */
		uint32 frameThreshold;
		RelocationPolicy policy;

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

		/** Implementation */
		virtual void Read(RenderContext* rc, void *dest, size_t offset = 0, size_t size = 0) override = 0;
		virtual void Write(RenderContext* rc, const void *src, size_t offset = 0, size_t size = 0) override = 0;
		virtual void CopyFrom(RenderContext* rc, BufferPtr&) override = 0;

	};

}

#endif //NEXTAR_VERTEXBUFFER_H
