
#include "NexHeaders.h"
#include "StringUtils.h"
#include "StringInternTable.h"

namespace nextar {

	StringInternTable::StringInternTable() {
	}

	StringInternTable::~StringInternTable() {
	}

	StringID StringInternTable::AsyncStringID(const String& value) {
#if !defined(NEX_HAS_CONCURRENT_CONTAINERS)
		NEX_THREAD_MUTEX(accessLock);
#endif // !defined(NEX_HAS_CONCURRENT_CONTAINERS)
		std::pair<StringSet::iterator, bool> it = listOfStrings.insert(value);
		return std::cref((*it.first));
	}
}