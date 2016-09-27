#ifndef STRINGINTERNTABLE_H_
#define	STRINGINTERNTABLE_H_

#include <NexBase.h>
#include <Logger.h>
#include <NexThread.h>
#include <StringUtils.h>

namespace nextar {

class _NexBaseAPI StringInternTable: public AllocGeneral {
	NEX_LOG_HELPER(StringInternTable)
	;
public:

	StringInternTable(const String& name);
	~StringInternTable();

	void SaveToCache();
	void LoadFromCache();
	void UnloadTable();	

	StringID AsyncStringID(const String& value);
	StringRef AsyncString(const StringID v);

protected:

	URL _GetURL();
	void _SaveToCache();
	void _LoadFromCache();
	void _Unload();

	typedef map<StringID, String>::type ForwardMap;
	typedef map<StringRef, StringID>::type ReverseMap;

	NEX_THREAD_MUTEX(accessLock);

	bool saveToCache;
	bool loadedFromCache;
	ForwardMap forwardMap;
	ReverseMap reverseMap;
	String name;
};
}

#endif // !STRINGINTERNTABLE_H_