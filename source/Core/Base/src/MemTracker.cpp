#include <BaseHeaders.h>
#include <Logger.h>
#include <Convert.h>
#include <NexThread.h>
#include <MemTracker.h>
#include <map>

#ifdef NEX_EXTENSIVE_MEM_TRACKER

#if defined( NEX_USE_PTMALLOC ) || !defined(NEX_MSVC)
#include <nedmalloc.h>
#define _nalloc       nedalloc::nedmalloc
#define _nfree        nedalloc::nedfree
#else
#define _nalloc       malloc
#define _nfree        free
#endif

namespace nextar {

namespace memtracker {
static char _IntfName[] = "MemTracker";

//@ this may look bad

struct MemNd {
	void* block;
	struct MemNd* next;
	long functiOn;
	long srcfile;
	size_t size;
	long line;
};

static MemNd* blockHead = 0;
static size_t totalSize = 0;
static size_t memBlocks = 0;

typedef std::map<long, std::string> RevStringMap;
typedef std::map<std::string, long> StringMap;
NEX_THREAD_MUTEX(safetyMute);

static StringMap funcmap;
static StringMap filemap;
static RevStringMap revfuncmap;
static RevStringMap revfilemap;

static long GetTicket(StringMap& smap, RevStringMap& rmap, const char* name) {

	static long ticket = 0;
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
// we could be more efficent
// by using the fast mem tracker technique, but for
// now we do not do that

void OnNewDbg(void* block, size_t size, const char* functiOn, const char* file,
		long line) {
	NEX_THREAD_LOCK_GUARD_MUTEX(safetyMute);
	char defval[] = "<unknown>";
	if (!functiOn)
		functiOn = defval;
	if (!file)
		file = defval;

	MemNd* newblock;
	newblock = (MemNd*) _nalloc(sizeof(MemNd));
	if (newblock) {
		newblock->block = block;

		newblock->functiOn = GetTicket(funcmap, revfuncmap, functiOn);
		newblock->srcfile = GetTicket(filemap, revfilemap, file);
		newblock->line = line;
		newblock->size = size;
		//	strcpy(newblock->srcfile,file);

		newblock->next = blockHead;

		blockHead = newblock;
		totalSize += size;
		memBlocks++;
	}
}

void OnDumpMemStats() {
	NEX_THREAD_LOCK_GUARD_MUTEX(safetyMute);
	LogManager::LogMsg(LOG_DEBUG, _IntfName,
			"Memory Status: Allocated Memory = "
					+ Convert::ToString((uint32) totalSize)
					+ ", Total Number of Blocks = "
					+ Convert::ToString((uint32) memBlocks));
}

int OnReleaseDbg(void* blk) {
	NEX_THREAD_LOCK_GUARD_MUTEX(safetyMute);
	MemNd* prev = 0;
	MemNd* mblock;
	int retv = 0;

	mblock = blockHead;
	while (mblock) {
		if (mblock->block == blk) {

			if (prev)
				prev->next = mblock->next;
			else
				blockHead = mblock->next;
			--memBlocks;
			totalSize -= mblock->size;
			_nfree((void*) mblock);
			retv = 1;
			break;
		}
		prev = mblock;
		mblock = mblock->next;
	}

	if (!retv) {
		NEX_THROW_FatalError(EXCEPT_DELETE_CALLED_ON_BAD_MEM);
	}
	return retv;
}

void CheckLeaks(std::ostream& outstream) {
	NEX_THREAD_LOCK_GUARD_MUTEX(safetyMute);
	if (blockHead) {
		MemNd* mblock = blockHead;
		while (mblock) {
			RevStringMap::iterator fn = revfuncmap.find(mblock->functiOn);
			RevStringMap::iterator fl = revfilemap.find(mblock->srcfile);

			outstream << _IntfName << ": " << std::endl << "  Leak: Address = "
					<< (size_t) mblock->block << std::endl << "  Func = "
					<< (*fn).second << std::endl << "  File = " << (*fl).second
					<< std::endl << "  Line = " << mblock->line << std::endl;
			mblock = mblock->next;
		}

		NEX_THROW_FatalError(EXCEPT_MEMORY_LEAK);
	}

	funcmap.clear();
	filemap.clear();
	revfuncmap.clear();
	revfilemap.clear();
}

}
}

#endif

