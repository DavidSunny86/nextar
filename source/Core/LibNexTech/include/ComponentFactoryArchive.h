/*
 * ComponentFactoryArchive.h
 *
 *  Created on: Oct 6, 2011
 *      Author: obhi
 */

#ifndef MANAGERROOT_H_
#define MANAGERROOT_H_

#include "Component.h"

namespace nextar {

	/*
	 *
	 */
	class ComponentFactoryArchive: public Singleton<ComponentFactoryArchive>,
		public AllocGeneral {
	public:

		ComponentFactoryArchive();
		virtual ~ComponentFactoryArchive();

		inline void AsyncRemoveFactory(const String& name) {
			AsyncRemoveFactory(Component::CLASS_UNKNOWN, name);
		}

		inline Component::Factory* AsyncFindFactory(uint32 componentType) {
			return AsyncFindFactory(componentType, StringUtils::Default);
		}

		void Configure(const Config&);

		void AsyncAddFactory(uint32 componentType, Component::Factory*);
		void AsyncRemoveFactory(uint32 componentType, const String&, bool deleteFactory = true);
		void AsyncDeleteAll();
		
		Component::Factory *AsyncFindFactory(uint32 componentType, const String& name);

	protected:

		typedef unordered_map<String, Component::Factory*>::type ComponentFactoryMap;

		NEX_THREAD_MUTEX(containerLock);
		ComponentFactoryMap componentFactories;
	};

} /* namespace nextar */
#endif /* MANAGERROOT_H_ */
