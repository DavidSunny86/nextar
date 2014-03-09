#include <BaseHeaders.h>

#if defined( NEX_EXTENSIVE_MEM_TRACKER  )
#	include "MemTracker.h"
#endif

#if defined( NEX_USE_PTMALLOC ) || !defined(NEX_MSVC)
#	undef   assert
#   include "nedmalloc_c.h"
#	define _nalloc       nedalloc::nedmalloc
#	define _nfree        nedalloc::nedfree
#	define _nalalloc(size, al)     nedalloc::nedmemalign(al, size)
#	define _nalfree		 nedalloc::nedfree
#else
#	define _nalloc       malloc
#	define _nfree        free
#	define _nalalloc     _aligned_malloc
#	define _nalfree		 _aligned_free
#endif

namespace nextar {
	void* G_CheckPtr = 0;
	bool G_TrackLeaks = false;

#ifdef NEX_EXTENSIVE_MEM_TRACKER
	void* Allocator::Alloc(size_t size, const char* func, const char* file,
			long line) {
		void* ptr = (void*) _nalloc(size);
		if (!ptr)
		NEX_THROW_FatalError(EXCEPT_OUT_OF_MEMORY);
		if (G_TrackLeaks)
		memtracker::OnNewDbg(ptr, size, func, file, line);
		return ptr;
	}

	void* Allocator::AllocAligned(size_t size, size_t al, const char* func,
			const char* file, long line) {
		void* ptr = (void*) _nalalloc(size,al);
		if (!ptr)
		NEX_THROW_FatalError(EXCEPT_OUT_OF_MEMORY);
		if (G_TrackLeaks)
		memtracker::OnNewDbg(ptr, size, func, file, line);
		return ptr;
	}

#endif

	void* Allocator::Alloc(size_t size) {
		return (void*) _nalloc(size);
	}

	void* Allocator::AllocAligned(size_t size, size_t al) {

		return (void*) _nalalloc(size,al);
	}

	void Allocator::Free(void* ptr) {
		NEX_ASSERT(G_CheckPtr != ptr);

		NEX_ASSERT(ptr);
#ifdef NEX_EXTENSIVE_MEM_TRACKER
		if (G_TrackLeaks)
			memtracker::OnReleaseDbg(ptr);
#endif
		_nfree(ptr);

	}

	void Allocator::FreeAligned(void* ptr) {
		NEX_ASSERT(ptr);
#ifdef NEX_EXTENSIVE_MEM_TRACKER
		if (G_TrackLeaks)
			memtracker::OnReleaseDbg(ptr);
#endif
		_nalfree(ptr);
	}

#ifdef NEX_EXTENSIVE_MEM_TRACKER
	_NexBaseExport void Allocator_CheckLeaks(std::ostream& s) {
		memtracker::CheckLeaks(s);
	}
	_NexBaseExport void Allocator_DumpMemStats(std::ostream& s) {
		memtracker::OnDumpMemStats();
	}

	_NexBaseExport void Allocator_DoTracking(bool track) {
		G_TrackLeaks = track;
	}
#endif

}

