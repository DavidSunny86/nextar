#ifndef NEX__MEMTRACKER_H
#define NEX__MEMTRACKER_H

//@do not include this file
#ifdef NEX_EXTENSIVE_MEM_TRACKER
namespace nextar {

	namespace memtracker {

		void OnNewDbg(void* block, size_t size, const char* functiOn, const char* file, long line);
		void OnDumpMemStats();
		int OnReleaseDbg(void* blk);
		void CheckLeaks(std::ostream&);

	}

}
#endif

#endif //NEX__MEMTRACKER_H
