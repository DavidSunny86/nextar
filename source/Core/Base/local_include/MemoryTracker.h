/*
 * MemoryTracker.h
 *
 *  Created on: 30-Jun-2014
 *      Author: obhi
 */

#ifndef MEMORYTRACKER_H_
#define MEMORYTRACKER_H_

#include <StdHeaders.h>
#ifdef NEX_EXTENSIVE_MEM_TRACKER

namespace nextar {

/*
 *
 */
class MemoryTracker {

	struct Node {
		const char* function;
		const char* srcfile;
		long line;
		size_t size;
		Node* next;
		Node* prev;
	};

public:

	enum {
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
		if (MEMTRACKER_BLOCK_SIZE & (alignment-1))
			return (MEMTRACKER_BLOCK_SIZE & (~(alignment-1))) + alignment;
		else
			return MEMTRACKER_BLOCK_SIZE;
	}

	static void* OnNewDbg(void* block, size_t size, const char* function, const char* file,
			long line, size_t alignment = DEFAULT_ALIGNMENT) {
		return Instance().OnNewDbg(block, size, function, file, line, alignment);
	}

	static void* OnReleaseDbg(void* blk) {
		return Instance().OnReleaseDbg(blk);
	}

	void DumpStats();
	void DumpLeaks();

protected:


	size_t currentAllocationSize;
	size_t totalAllocations;
	size_t maxAllocationsAtAnyTime;
	Node* _head;
	std::mutex _lock;
};

} /* namespace RenderOpenGL */

#endif
#endif /* MEMORYTRACKER_H_ */
