
#include "NexHeaders.h"
#include "NamedObject.h"

namespace nextar {


#ifdef NEX_USE_STRING_NAMES
	StringInternTable NamedObject::nameTable;
	NamedObject::NamedObject(const String& _name) : 
		name(nameTable.AsyncStringID(_name)) {
	}
#endif
}
