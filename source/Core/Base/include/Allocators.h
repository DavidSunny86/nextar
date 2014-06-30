#ifndef NEXTAR_NEXALLOC_H
#error Include NexAlloc.h instead
#endif

#include <StdHeaders.h>

namespace nextar {
//@ normal allocator
//@ debug allocator

class _NexBaseAPI Allocator {
public:

#ifdef NEX_EXTENSIVE_MEM_TRACKER
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
#ifdef NEX_EXTENSIVE_MEM_TRACKER

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
		Free(ptr);
	}

};

template<const size_t alignment = 16>
class Aligned {
public:
#ifdef NEX_EXTENSIVE_MEM_TRACKER

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
		Free(ptr);
	}
};
}

}

