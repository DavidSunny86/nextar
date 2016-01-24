#include <BaseHeaders.h>
#include <NamedObject.h>

namespace nextar {

StringInternTable NamedObject::nameTable("NamedObject");

NamedObject::NamedObject(const String& _name) :
		name(nameTable.AsyncStringID(_name)) {
}

StringID NamedObject::AsyncStringID(const String& name) {
	return nameTable.AsyncStringID(name);
}

void NamedObject::OnExit() {
	nameTable.SaveToCache();
	nameTable.UnloadTable();
}

void NamedObject::OnFlushStrings() {
	nameTable.UnloadTable();
}

}
