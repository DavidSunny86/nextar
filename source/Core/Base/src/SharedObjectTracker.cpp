//
// File Name :: iobject.cpp
// Author    :: Obi De
// Created   :: 7/5/2006 5:08:06 AM
//-------------------------------------------------------------------------------------

#include "BaseHeaders.h"
#include "SharedObjectTracker.h"

using namespace nextar;
#ifdef NEX_TRACK_OBJECTS 
using namespace nextar::debug;

#include "stdio.h"

struct AllocBlockNode {
	void* obj;
	const char* name;
	AllocBlockNode* next;
};

static AllocBlockNode* head = 0;
uint32 count = 0;
static NEX_THREAD_MUTEX(safety_mutex);

void DebugUnkownTracker::OnCtor(void* obj, const char* name) {
	// if rtti is on we can get good info.
	AllocBlockNode* nnd =
	(AllocBlockNode*) NEX_ALLOC(sizeof (AllocBlockNode), MEMCAT_GENERAL);
	nnd->obj = obj;
	nnd->name = name;
	NEX_THREAD_LOCK_GUARD_MUTEX(safety_mutex);
	count++;
	nnd->next = head;
	head = nnd;
}

void DebugUnkownTracker::OnDtor(void* obj) {
	NEX_THREAD_LOCK_GUARD_MUTEX(safety_mutex);
	AllocBlockNode* prv = 0;
	AllocBlockNode* nnd = head;
	while (nnd) {
		if (nnd->obj == obj) {
			if (prv)
				prv->next = nnd->next;
			else
				head = nnd->next;
			--count;
			NEX_FREE(nnd, MEMCAT_GENERAL);
			break;
		}
		prv = nnd;
		nnd = nnd->next;
	}
}

void DebugUnkownTracker::Dump(std::ostream& outstream) {
	NEX_THREAD_LOCK_GUARD_MUTEX(safety_mutex);
	if (count) {
		AllocBlockNode* nnd = head;
		while (nnd) {
			outstream << "Object: " << nnd->name << std::endl;
			nnd = nnd->next;
		}
	}
}

void DebugUnkownTracker::LeakCheck(std::ostream& outstream) {
	if (count) {
		outstream << "Leaks detected:" << std::endl;
		Dump(outstream);
	}
}

#endif

