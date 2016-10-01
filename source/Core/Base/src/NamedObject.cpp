#include <BaseHeaders.h>
#include <NamedObject.h>
#include <ApplicationContext.h>

namespace nextar {

NamedObject::TableStack NamedObject::nameTables;

NamedObject::TableStack::TableStack() : _current(nullptr) {
}

NamedObject::TableStack::~TableStack() {
	// this is a mistake
	OnExit();
}

void NamedObject::TableStack::Initialize() {
	NEX_ASSERT(!_current);
	_current = NEX_NEW(StringInternTable(ApplicationContext::Instance().GetProjectName()));
}

StringID NamedObject::TableStack::AsyncStringID(const String& name) {
	if (!_current)
		Initialize();
	return _current->AsyncStringID(name);
}

const String& NamedObject::TableStack::AsyncString(StringID name) {
	if (!_current)
		Initialize();
	return _current->AsyncString(name);
}

void NamedObject::TableStack::PushContext(const String& name) {
	if (_current)
		_tables.push_back(_current);
	_current = NEX_NEW(StringInternTable(name));
}

void NamedObject::TableStack::PopContext(bool flush) {
	if (_current) {
		if (flush) {
			_current->SaveToCache();
			_current->UnloadTable();
		}
		NEX_DELETE( _current);
		_current = nullptr;
	}

	if (_tables.size()) {
		_current = _tables.back();
		_tables.pop_back();
	}
}

void NamedObject::TableStack::OnFlushStrings() {
	if (_current) {
		_current->UnloadTable();
	}
	for (auto& t : _tables) {
		t->UnloadTable();
	}
}

void NamedObject::TableStack::OnExit() {

	if (_current) {
		_tables.push_back(_current);
	}

	while (_tables.size()) {
		_current = _tables.back();
		_current->SaveToCache();
		_current->UnloadTable();
		NEX_DELETE(_current);
		_current = nullptr;
		_tables.pop_back();
	}
	StringTableList s;
	_tables.swap(s);
}

NamedObject::NamedObject(const String& _name) :
		name(nameTables.AsyncStringID(_name)) {
}

StringID NamedObject::AsyncStringID(const String& name) {
	return nameTables.AsyncStringID(name);
}

void NamedObject::OnExit() {
	nameTables.OnExit();
}

void NamedObject::OnFlushStrings() {
	nameTables.OnFlushStrings();
}

}
