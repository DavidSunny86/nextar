
#include "NexHeaders.h"
#include "NamedObject.h"

namespace nextar {

	StringInternTable NamedObject::nameTable;

	NamedObject::NamedObject(const String& _name) : 
		name(nameTable.AsyncStringID(_name)) {
	}
}