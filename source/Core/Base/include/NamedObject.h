#ifndef NAMEDOBJECT_H_
#define NAMEDOBJECT_H_

#include <StringUtils.h>
#include <Convert.h>

namespace nextar {

class _NexBaseAPI NamedObject {
public:

	NamedObject(const StringID _name) :
			name(_name) {
	}
	inline NamedObject() :
			name(StringUtils::NullID) {
	}
	inline NamedObject(const String& iName) : name(StringUtils::Hash(iName)) {}

	inline void SetName(const String& name) {
		this->name = StringID(StringUtils::Hash(name));
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

protected:
	/** todo Must be a string ID */
	StringID name;
};

}

#endif // !NAMEDOBJECT_H_