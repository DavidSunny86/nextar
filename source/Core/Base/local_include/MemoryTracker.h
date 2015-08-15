/*
 * MemoryTracker.h
 *
 *  Created on: 30-Jun-2014
 *      Author: obhi
 */

#ifndef MEMORYTRACKER_H_
#define MEMORYTRACKER_H_

#include <StdHeaders.h>
#include <NexThread.h>

#ifdef NEX_EXTENSIVE_MEM_TRACKER

namespace nextar {

/*
 *
 */
class MemoryTracker {

	struct Node {
		int function;
		int srcfile;
		int line;
		void* address;
		size_t size;
		Node* next;
		Node* prev;
	};

public:

	enum {
		ENABLED = 1,
		DEFAULT_ALIGNMENT = 8,
		MEMTRACKER_BLOCK_SIZE = sizeof(Node)+sizeof(void*)
	};

	static MemoryTracker& Instance();

	MemoryTracker();
	virtual ~MemoryTracker();

	void* OnNewDbg(void* block, size_t size, const char* functiOn, const char* file,
			long line, size_t alignment);
	void* OnReleaseDbg(void* blk);

	inline static size_t GetBlockSize(const size_t alignment = DEFAULT_ALIGNMENT) {
		if (!ENABLED)
			return 0;
		if (MEMTRACKER_BLOCK_SIZE & (alignment-1))
			return (MEMTRACKER_BLOCK_SIZE & (~(alignment-1))) + alignment;
		else
			return MEMTRACKER_BLOCK_SIZE;
	}

	static void* OnNew(void* block, size_t size, const char* function, const char* file,
			long line, size_t alignment = DEFAULT_ALIGNMENT) {
		if (!ENABLED)
			return block;
		return Instance().OnNewDbg(block, size, function, file, line, alignment);
	}

	static void* OnRelease(void* block) {
		if (!ENABLED)
			return block;
		return Instance().OnReleaseDbg(block);
	}

	
	void DumpStats();
	void DumpLeaks();
	
protected:


	size_t currentAllocationSize;
	size_t totalAllocations;
	size_t maxAllocationsAtAnyTime;
	Node* _head;
	spinlock_mutex _lock;
	typedef std::map<long, std::string> RevStringMap;
	typedef std::map<std::string, long> StringMap;
	long GetTicket(StringMap& smap, RevStringMap& rmap, const char* name);

	RevStringMap revfilemap;
	RevStringMap revfuncmap;
	StringMap filemap;
	StringMap funcmap;
	long ticket;
	typedef std::lock_guard<spinlock_mutex> LockGuard;
};

} /* namespace RenderOpenGL */

#endif
#endif /* MEMORYTRACKER_H_ */
