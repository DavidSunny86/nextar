#ifndef NEXTAR_GRAPHICS_BUFFER_H
#define NEXTAR_GRAPHICS_BUFFER_H

#include <NexBase.h>
#include <ContextObject.h>

namespace nextar {

class _NexEngineAPI GpuBuffer: public ContextObject,
		public Referenced<GpuBuffer, AllocGeneral> {
public:
	enum RelocationPolicy
		: uint32 {
		INVALID_POLICY,
			// Forever: Static meshes, immutable
		NEVER_RELEASED,
		// Long Lived: Characters, default
		SELDOM_RELEASED,
		// Transient: Text, UI, multi-buffered
		REGULARLY_RELEASED,
		// Temporary: particles, multi-buffered
		IMMEDIATELY_RELEASED,
	};

	enum Message {
		MSG_CREATE, MSG_OP_READ, MSG_OP_WRITE,
	};

	struct CreateParams {
		const uint8* sourceData;
		RelocationPolicy policy;
		size_t size;
		uint32 elementStride;
	};

	struct ReadParams {
		void* bufferPtr;
		size_t offset;
		size_t size;
	};

	struct WriteParams {
		const void* bufferPtr;
		size_t offset;
		size_t size;
	};

	struct MapResult {
		uint8* data;
	};

	class _NexEngineAPI View: public ContextObject::View {
	protected:
		virtual void Update(nextar::RenderContext*, uint32 msg,
				ContextParamPtr);
		virtual void Create(RenderContext* rc, size_t size,
				uint32 elementStride,
				const void* dataPtr, RelocationPolicy policy) = 0;
		virtual void Read(RenderContext* rc, void *dest, size_t offset,
				size_t size) = 0;
		virtual void Write(RenderContext* rc, const void *src, size_t offset,
				size_t size) = 0;
		virtual MapResult Map(RenderContext* rc, size_t offset = 0, size_t size = 0) = 0;
		virtual void Unmap(RenderContext* rc) = 0;
	};

	GpuBuffer() : size(0) {}
	GpuBuffer(GpuBuffer&& other);
	GpuBuffer(ContextObject::Type type, RelocationPolicy policy) :
			ContextObject(type, (uint32)policy), size(0) {
	}

	virtual ~GpuBuffer() {
	}
	/** This function will fail unless a local copy of the data exists. The local
	 * copy is created if the CPU_READ flag is set. **/
	virtual void Read(void *dest, size_t offset = 0, size_t size = 0);
	/** Write to buffer */
	virtual void Write(const void *src, size_t offset = 0, size_t size = 0);

	inline size_t GetSize() const {
		return size;
	}

	inline GpuBuffer& operator = (GpuBuffer&& other) {
		ContextObject::operator =(std::move(other));
		size = other.size;
		other.size = 0;
		return *this;
	}

protected:
	size_t size;
};

}

#endif //NEXTAR_GRAPHICS_BUFFER_H
