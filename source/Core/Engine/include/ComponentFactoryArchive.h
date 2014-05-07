/*
 * ComponentFactoryArchive.h
 *
 *  Created on: Oct 6, 2011
 *      Author: obhi
 */

#ifndef MANAGERROOT_H_
#define MANAGERROOT_H_

#include <Component.h>

namespace nextar {

	/*
	 *
	 */
	class ComponentFactoryArchive: public Singleton<ComponentFactoryArchive>,
		public AllocGeneral {
	public:

		ComponentFactoryArchive();
		virtual ~ComponentFactoryArchive();

		inline void AsyncRemoveFactory(const StringID name) {
			AsyncRemoveFactory(Component::CLASS_UNKNOWN, name);
		}

		inline Component::Factory* AsyncFindFactory(uint32 componentType) {
			return AsyncFindFactory(componentType, StringUtils::DefaultID);
		}

		void Configure(const Config&);

		void AsyncAddFactory(uint32 componentType, Component::Factory*);
		void AsyncRemoveFactory(uint32 componentType, StringID, bool deleteFactory = true);
		void AsyncDeleteAll();
		
		Component::Factory *AsyncFindFactory(uint32 componentType, const StringID name);

	protected:

		typedef std::pair<StringID, uint32> FactoryID;

		inline friend bool operator < (const FactoryID& id1, const FactoryID& id2) {
			return id1.first == id2.first?
					(id1.second < id2.second) != 0 :
					(id1.first < id2.first) != 0;
		}

		inline friend bool operator == (const FactoryID& id1, const FactoryID& id2) {
			return (id1.first == id2.first && id1.second == id2.second) != 0;
		}

		typedef map<FactoryID, Component::Factory*>::type ComponentFactoryMap;

		NEX_THREAD_MUTEX(containerLock);
		ComponentFactoryMap componentFactories;
	};

} /* namespace nextar */
#endif /* MANAGERROOT_H_ */
