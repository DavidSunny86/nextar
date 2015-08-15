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
		maxAllocationsAtAnyTime(0),
		ticket(0) {
}

MemoryTracker::~MemoryTracker() {
	DumpStats();
	DumpLeaks();
}

MemoryTracker& MemoryTracker::Instance() {
	static MemoryTracker tracker;
	return tracker;
}

long MemoryTracker::GetTicket(StringMap& smap, RevStringMap& rmap, const char* name) {
	long ret;
	std::string stdname = name;
	StringMap::iterator it = smap.find(stdname);
	if (it == smap.end()) {
		smap.insert(StringMap::value_type(stdname, ticket));
		rmap.insert(RevStringMap::value_type(ticket, stdname));
		ret = ticket++;
	} else
		ret = (*it).second;
	return ret;
}

void* MemoryTracker::OnNewDbg(void* block, size_t size, const char* functiOn, const char* file,
		long line, size_t alignment) {
	size_t blockSize = GetBlockSize(alignment);
	uint8* blockAdd = reinterpret_cast<uint8*>(block);
	Node* nodeAdd = reinterpret_cast<Node*>(block);
	void** storeBeginAdd = reinterpret_cast<void**>((blockAdd + blockSize) - sizeof(void*));
	*storeBeginAdd = blockAdd;
	nodeAdd->function = GetTicket(funcmap, revfuncmap, functiOn);
	nodeAdd->line = line;
	nodeAdd->size = size;
	nodeAdd->srcfile = GetTicket(filemap, revfilemap, file);
	nodeAdd->prev = nullptr;
	nodeAdd->address = (blockAdd + blockSize);
	LockGuard guard(_lock);
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
	void* beginAdd = *(reinterpret_cast<void**>((blockRem) - sizeof(void*)));
	Node* nodeRem = reinterpret_cast<Node*>(beginAdd);
	LockGuard guard(_lock);
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
	LockGuard guard(_lock);
	std::ostringstream stream;
	stream << "\n*****************************************"
			  "\n  Memory Status:"
		      "\n  Total Allocated Memory = " << totalAllocations
		   << "\n  Max Allocations At Any Time = " << maxAllocationsAtAnyTime
		   << "\n  Current Allocations = " << currentAllocationSize
		   << "\n*****************************************\n";
	Platform::OutputDebug(stream.str().c_str());
}

void MemoryTracker::DumpLeaks() {
	LockGuard guard(_lock);
	if (_head) {

		Platform::OutputDebug("\nLeaks detected!"
							  "\n------------------------------------");
		while(_head) {
			RevStringMap::iterator src = revfilemap.find(_head->srcfile);
			RevStringMap::iterator fn = revfuncmap.find(_head->function);

			std::ostringstream stream;
			stream << "\nLeak:>>\n  file: " << (*src).second
						 <<  "\n  function: " << (*fn).second
						 <<  ",  line: " << _head->line
						 <<  "\n  size: " << _head->size
						 <<  ",  address: " << _head->address
						 <<  ", content: " << (const char*)_head->address;
			Platform::OutputDebug(stream.str().c_str());
			_head = _head->next;
		}
		Platform::OutputDebug("\n------------------------------------\n");
		NEX_THROW_FatalError(EXCEPT_MEMORY_LEAK);
	}
}


} /* namespace RenderOpenGL */
#endif

