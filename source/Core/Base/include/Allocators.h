#ifndef NEXTAR_NEXALLOC_H
#error Include NexAlloc.h instead
#endif

#include <StdHeaders.h>

namespace nextar {
//@ normal allocator
//@ debug allocator

	class _NexBaseExport Allocator {
	public:

#if NEX_MEMTRACKERLEVEL > 1
		static void* Alloc(size_t size, const char* func, const char* file, long line);
		static void* AllocAligned(size_t size, size_t alignment, const char* func, const char* file, long line);
#endif
		static void* Alloc(size_t size);
		static void* AllocAligned(size_t size, size_t alignment);

		static void Free(void*);
		static void FreeAligned(void*);

	private:

		_NexInline Allocator() {
		}
	};

	namespace allocator {

		class Default {
		public:
#if NEX_MEMTRACKERLEVEL > 1

			static inline void* Alloc(size_t size,
					const char* func,
					const char* file,
					long line) {
				return Allocator::Alloc(size, func, file, line);
			}
#endif						
			static inline void* Alloc(size_t size) {
				return Allocator::Alloc(size);
			}
						
			static inline void Free(void* ptr) {
				Allocator::Free(ptr);
			}

			static inline void Free(void* ptr, size_t size) {
				size;
				Free(ptr);
			}

		};

		template<const size_t alignment = 16>
		class Aligned {
		public:
#if NEX_MEMTRACKERLEVEL > 1

			static inline void* Alloc(size_t size,
					const char* func,
					const char* file,
					long line) {
				return Allocator::AllocAligned(size, alignment, func, file, line);
			}
#endif
						
			static inline void* Alloc(size_t size) {
				return Allocator::AllocAligned(size, alignment);
			}

			static inline void Free(void* ptr) {
				return Allocator::FreeAligned(ptr);
			}

			static inline void Free(void* ptr, size_t size) {
				size;
				Free(ptr);
			}
		};
	}

#if NEX_MEMTRACKERLEVEL > 1
_NexBaseExport void Allocator_DoTracking(bool);
_NexBaseExport void Allocator_CheckLeaks(std::ostream& s);
_NexBaseExport void Allocator_DumpMemStats();
#define NEXTAR_START_LEAK_TRACKING()		nextar::Allocator_DoTracking(true)
#define NEXTAR_STOP_LEAK_TRACKING()		nextar::Allocator_DoTracking(false)
#define NEXTAR_LEAK_CHECKS(outstream)	nextar::Allocator_CheckLeaks(outstream)
#else 
#define Allocator_DoTracking()
#define Allocator_CheckLeaks(o)
#define Allocator_DumpMemStats()
#define NEXTAR_LEAK_CHECKS(o)
#define NEXTAR_START_LEAK_TRACKING()
#define NEXTAR_STOP_LEAK_TRACKING()
#endif

}

