
#ifndef NAMEDOBJECT_H_
#define NAMEDOBJECT_H_

#include "StringUtils.h"

namespace nextar {

	class NamedObject {
	public:

		NamedObject(const StringID _name) : name(_name) {}
		inline NamedObject() : name(StringUtils::NullID) {
		}
#ifdef NEX_USE_STRING_NAMES
		NamedObject(const String& name);
		inline void SetName(const String& name) {
			this->name = nameTable.AsyncStringID(name);
		}

		inline const String& GetName() const {
			return nameTable.AsyncString(name);
		}
#else
		inline const String& GetName() const {
			return StringUtils::Unknown;
		}
#endif
		inline void SetID(StringID name) {
			this->name = name;
		}

		inline StringID GetID() const {
			return name;
		}

	protected:
#ifdef NEX_USE_STRING_NAMES
		static StringInternTable nameTable;
#endif
		/** todo Must be a string ID */
		StringID name;
	};
}

#endif // !NAMEDOBJECT_H_
