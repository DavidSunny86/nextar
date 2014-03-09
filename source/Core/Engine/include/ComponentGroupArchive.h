/*
 * ComponentGroupArchive.h
 *
 *  Created on: 01-Mar-2014
 *      Author: obhi
 */

#ifndef COMPONENTGROUPARCHIVE_H_
#define COMPONENTGROUPARCHIVE_H_

#include <Component.h>

namespace nextar {

	/*
	 *
	 */
	class ComponentGroupArchive : public Singleton<ComponentGroupArchive>,
		public AllocGeneral {
	public:
		ComponentGroupArchive();
		virtual ~ComponentGroupArchive();

		void Configure(const Config&);

		void AsyncAddGroup(SharedComponent::Group*);
		void AsyncRemoveGroup(StringID name, bool del = true);
		SharedComponent::Group* AsyncFindGroup(StringID name);
		void AsyncDeleteAll();

		inline SharedComponent::Group* AsyncFindOrCreate(StringID name) {
			if (name == StringUtils::DefaultID && defaultGroup)
				return defaultGroup;
			return _AsyncFindOrCreate(name);
		}

		inline SharedComponent::Group* AsyncFindGroup(StringID name) {
			if (name == StringUtils::DefaultID)
				return defaultGroup;
			return _AsyncFindGroup(name);
		}

	protected:
		SharedComponent::Group* _AsyncFindGroup(StringID name);
		SharedComponent::Group* _AsyncFindOrCreate(StringID name);

		typedef map<StringID, SharedComponent::Group*>::type ComponentGroupMap;
		NEX_THREAD_MUTEX(containerLock);
		SharedComponent::Group* defaultGroup;
		ComponentGroupMap componentGroups;
	};

} /* namespace nextar */

#endif /* COMPONENTGROUPARCHIVE_H_ */
