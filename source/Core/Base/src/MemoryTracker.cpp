/*
 * MemoryTracker.cpp
 *
 *  Created on: 30-Jun-2014
 *      Author: obhi
 */

#include <NexBase.h>
#include <MemoryTracker.h>
#include <iostream>

#ifdef NEX_EXTENSIVE_MEM_TRACKER
namespace nextar {

MemoryTracker::MemoryTracker() : _head(nullptr),
		currentAllocationSize(0),
		totalAllocations(0),
		maxAllocationsAtAnyTime(0) {
}

MemoryTracker::~MemoryTracker() {
	DumpStats();
	DumpLeaks();
}

MemoryTracker& MemoryTracker::Instance() {
	static MemoryTracker tracker;
	return tracker;
}

void* MemoryTracker::OnNewDbg(void* block, size_t size, const char* functiOn, const char* file,
		long line, size_t alignment) {
	size_t blockSize = GetBlockSize(alignment);
	uint8* blockAdd = reinterpret_cast<uint8*>(block);
	Node* nodeAdd = reinterpret_cast<Node*>(block);
	void** storeBeginAdd = static_cast<void**>((blockAdd + blockSize) - sizeof(void*));
	*storeBeginAdd = blockAdd;
	nodeAdd->function = functiOn;
	nodeAdd->line = line;
	nodeAdd->size = size;
	nodeAdd->srcfile = file;
	nodeAdd->prev = nullptr;

	std::lock_guard guard(_lock);
	if(_head)
		_head->prev = nodeAdd;
	currentAllocationSize += size;
	totalAllocations += size;
	if(maxAllocationsAtAnyTime < currentAllocationSize)
		maxAllocationsAtAnyTime = currentAllocationSize;
	nodeAdd->next = _head;
	_head = nodeAdd;
	return (blockAdd + blockSize);
}

void* MemoryTracker::OnReleaseDbg(void* block) {
	uint8* blockRem = reinterpret_cast<uint8*>(block);
	void* beginAdd = *(static_cast<void**>((blockRem) - sizeof(void*)));
	Node* nodeRem = reinterpret_cast<Node*>(beginAdd);
	std::lock_guard guard(_lock);
	if (nodeRem->prev)
		nodeRem->prev->next = nodeRem->next;
	if (nodeRem->next)
		nodeRem->next->prev = nodeRem->prev;
	if (nodeRem == _head)
		_head = nodeRem->next;
	currentAllocationSize -= nodeRem->size;
	return beginAdd;
}

void MemoryTracker::DumpStats() {
	std::lock_guard guard(_lock);
	std::ostringstream stream;
	stream << "*****************************************"
			  "\n  Memory Status:"
		      "\n  Total Allocated Memory = " << totalAllocations
		   << "\n  Max Allocations At Any Time = " << maxAllocationsAtAnyTime
		   << "\n  Current Allocations = " << currentAllocationSize
		   << "*****************************************";
	Platform::OutputDebug(stream.str().c_str());
}

void MemoryTracker::DumpLeaks() {
	std::lock_guard guard(_lock);
	if (_head) {
		Platform::OutputDebug("Leaks detected!");
		while(_head) {
			std::ostringstream stream;
			stream << "Leak:>>\n  file: " << _head->srcfile
						 <<  "\n  function: " << _head->function
						 <<  "\n  line: " << _head->line
						 <<  "\n  size: " << _head->size;
			Platform::OutputDebug(stream.str().c_str());
			_head = _head->next;
		}
		NEX_THROW_FatalError(EXCEPT_MEMORY_LEAK);
	}
}


} /* namespace RenderOpenGL */
#endif

