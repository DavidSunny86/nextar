#ifndef NAMEDOBJECT_H_
#define NAMEDOBJECT_H_

#include <StringUtils.h>
#include <Convert.h>

namespace nextar {

class _NexBaseAPI NamedObject {
public:

	typedef vector<StringInternTable*>::type StringTableList;
	class TableStack {

	public:

		TableStack();
		~TableStack();

		void Initialize();
		StringID AsyncStringID(const String& name);
		const String& AsyncString(StringID);
		void PushContext(const String& name);
		void PopContext(bool flush);
		void OnExit();
		void OnFlushStrings();

		StringTableList _tables;
		StringInternTable* _current;
	};

	NamedObject(const StringID _name) :
			name(_name) {
	}
	inline NamedObject() :
			name(StringUtils::NullID) {
	}
	NamedObject(const String& name);

	inline void SetName(const String& name) {
		this->name = nameTables.AsyncStringID(name);
	}

	inline const String& GetName() const {
		return nameTables.AsyncString(name);
	}

	inline void SetID(StringID name) {
		this->name = name;
	}

	inline StringID GetID() const {
		return name;
	}

	inline String GetNameID() const {
		return Convert::ToString(name);
	}

	static void PushContext(const String& name) {
		nameTables.PushContext(name);
	}

	static void PopContext(bool flush) {
		nameTables.PopContext(flush);
	}

	static StringID AsyncStringID(const String&);
	static void OnExit();
	static void OnFlushStrings();

protected:

	static TableStack nameTables;
	/** todo Must be a string ID */
	StringID name;
};

}

#endif // !NAMEDOBJECT_H_