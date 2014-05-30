#ifndef NEXTAR_GRAPHICS_BUFFER_H
#define NEXTAR_GRAPHICS_BUFFER_H

#include <NexBase.h>
#include <ContextObject.h>

namespace nextar {

	class GpuBuffer:
			public ContextObject,
			public Referenced<GpuBuffer, AllocGeneral> {
	public:
		enum Flags {
			GPU_READ = 1 << 0,
			GPU_WRITE = 1 << 1,
			CPU_READ = 1 << 2,
			CPU_WRITE = 1 << 3,
			LAST_FLAG = 1 << 4,
			GPU_ACCESS = GPU_READ | GPU_WRITE,
			CPU_ACCESS = CPU_READ | CPU_WRITE,
			DYNAMIC = CPU_WRITE | GPU_READ,
			DEFAULT_FLAGS = GPU_ACCESS,
		};

		enum Op {
			OP_READ,
			OP_WRITE,
		};

		struct ReadStruct {
			void* bufferPtr;
			size_t offset;
			size_t size;
		};

		struct WriteStruct {
			const void* bufferPtr;
			size_t offset;
			size_t size;
		};

		struct ContextParam {
			uint8 operation;
			union {
				ReadStruct readBox;
				WriteStruct writeBox;
			};
		};

		class View : public ContextObject::View {
		protected:
			virtual void Update(nextar::RenderContext*, ContextObject::ContextParamPtr);
			virtual void Read(RenderContext* rc, void *dest, size_t offset, size_t size) = 0;
			virtual void Write(RenderContext* rc, const void *src, size_t offset, size_t size) = 0;
			virtual void CopyFrom(RenderContext* rc, BufferPtr&) = 0;

		};

		GpuBuffer(size_t bufferSize, uint32 flags) : creator(0),
				size(bufferSize), flags(flags) {
		}

		virtual ~GpuBuffer() {
		}
		/** This function will fail unless a local copy of the data exists. The local
		 * copy is created if the CPU_READ flag is set. **/
		virtual void Read(void *dest, size_t offset = 0, size_t size = 0);
		/** Write to buffer */
		virtual void Write(const void *src, size_t offset = 0, size_t size = 0);

		inline BufferManager *GetCreator() const {
			return creator;
		}

		inline size_t GetSize() const {
			return size;
		}

		inline void SetCreator(BufferManager *creator) {
			this->creator = creator;
		}

		inline uint32 GetFlags() const {
			return flags;
		}

		virtual void SetFlags(uint32 accessFlags) {
			this->flags = accessFlags;
		}

	protected:

		BufferManager* creator;
		uint32 flags;
		size_t size;
	};
}

#endif //NEXTAR_GRAPHICS_BUFFER_H
