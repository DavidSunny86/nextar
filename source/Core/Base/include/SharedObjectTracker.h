//@ Obi De
//@ 10/28/2006
//@ 1/7/2007
// Aug 03 07
//@ Oct 18 2007
//@ Dec 24 07
//@ original tracker used to keep a single linked list
//@ of objects in its own domain.
#ifndef NEXTAR_ISHAREDOBJECTTRACKER
#define NEXTAR_ISHAREDOBJECTTRACKER

#ifdef NEX_TRACK_OBJECTS
#define  dbg_BreakFeature() public: bool dbg_Break_Ctrl
#define	dbg_Ctor(ref)	dbg_Break_Ctrl = false; int status; const char* c_typeName = NEX_CLEAN_TYPE_NAME(*this, status); debug::DebugUnkownTracker::OnCtor(this,c_typeName)
#define	dbg_Dtor()		debug::DebugUnkownTracker::OnDtor(this)
#define	dbg_IncRef()	if(dbg_Break_Ctrl) NEX_DEBUG_BREAK()
#define	dbg_DecRef()	if(dbg_Break_Ctrl) NEX_DEBUG_BREAK()
#define	dbg_Dispose()	//debug::DebugUnkownTracker::Dispose(this)#define	dbg_Track_RefChanges() dbg_Break_Ctrl = true
#define  NEXTAR_UNK_TRACKED_OBJS(outstream) nextar::debug::DebugUnkownTracker::LeakCheck(outstream)

namespace nextar {

	namespace debug {

		class _NexBaseAPI DebugUnkownTracker {
			//NEX_LOG_HELPER(DebugUnkownTracker);
		public:
			static void OnCtor(void* obj, const char* name);
			static void OnDtor(void* obj);
			static void Dump(std::ostream&);
			static void LeakCheck(std::ostream&);
		};

	}

}

#else
#define  dbg_BreakFeature()
#define	dbg_Ctor(ref)
#define	dbg_Dtor()
#define	dbg_IncRef()
#define	dbg_DecRef()
#define	dbg_Dispose()
#define	dbg_Track_RefChanges()
#define  NEXTAR_UNK_TRACKED_OBJS(o)

#endif

#endif

