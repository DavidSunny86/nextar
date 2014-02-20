
#ifndef NAMEDOBJECT_H_
#define NAMEDOBJECT_H_

#include "StringUtils.h"

namespace nextar {

	class NamedObject {
	public:

		NamedObject(const String& name);
		inline NamedObject() : name(StringUtils::NullID) {
		}
		
		inline void SetName(const String& name) {
			this->name = nameTable.AsyncStringID(name);
		}

		inline const String& GetName() const {
			return nameTable.AsyncString(name);
		}

		inline StringID GetID() const {
			return name;
		}

	protected:
		
		static StringInternTable nameTable;

		/** todo Must be a string ID */
		StringID name;
	};
}

#endif // !NAMEDOBJECT_H_
