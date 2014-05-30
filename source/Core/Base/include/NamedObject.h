
#ifndef NAMEDOBJECT_H_
#define NAMEDOBJECT_H_

#include <StringUtils.h>

namespace nextar {

	class _NexBaseAPI NamedObject {
	public:

		NamedObject(const StringID _name) : name(_name) {}
		inline NamedObject() : name(StringUtils::NullID) {
		}
		NamedObject(const String& name);

		inline void SetName(const String& name) {
			this->name = nameTable.AsyncStringID(name);
		}

		inline const String& GetName() const {
			return nameTable.AsyncString(name);
		}

		inline void SetID(StringID name) {
			this->name = name;
		}

		inline StringID GetID() const {
			return name;
		}

		inline String GetNameID() const {
			return Convert::ToString((uint32)name);
		}

		static StringID AsyncStringID(const String&);

	protected:
		static StringInternTable nameTable;
		/** todo Must be a string ID */
		StringID name;
	};
}

#endif // !NAMEDOBJECT_H_
