
#ifndef STRINGINTERNTABLE_H_
#define	STRINGINTERNTABLE_H_


#include "NexTypes.h"
#include "NexThread.h"

namespace nextar {
		
	typedef std::reference_wrapper<const String> StringID;
	
	inline bool operator == (const StringID& first, const StringID& second) {
		return (&first.get()) == (&second.get());
	}

	inline bool operator < (const StringID& first, const StringID& second) {
		return (&first.get()) < (&second.get());
	}

	class _NexExport StringInternTable {
	public:

		StringInternTable();
		~StringInternTable();

		StringID AsyncStringID(const String& value);

		inline const String& AsyncString( const StringID& v ) const {
			return v.get();
		}

	protected:
#if defined(NEX_HAS_CONCURRENT_CONTAINERS)
		typedef concurrent_unordered_set<String>::type StringSet;
#else
		typedef set<String>::type StringSet;
		NEX_THREAD_MUTEX(accessLock);
#endif
		/* In release mode, might just offload this list to a disk as it wont be required in most cases */
		StringSet	listOfStrings;
	};
}


#endif // !STRINGINTERNTABLE_H_
