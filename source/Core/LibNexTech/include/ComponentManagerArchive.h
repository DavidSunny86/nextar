/*
 * ManagerRoot.h
 *
 *  Created on: Oct 6, 2011
 *      Author: obhi
 */

#ifndef MANAGERROOT_H_
#define MANAGERROOT_H_

#include "ComponentManager.h"

namespace nextar {

	/*
	 *
	 */
	class ComponentManagerArchive: public Singleton<ComponentManagerArchive>,
		public AllocGeneral {
	public:

		ComponentManagerArchive();
		virtual ~ComponentManagerArchive();

		inline void AsyncRemoveManager(const String& name) {
			AsyncRemoveManager(Component::COMPONENT_UNKNOWN, name);
		}

		inline ComponentManager* AsyncFindManager(int componentType) {
			return AsyncFindManager(componentType, StringUtils::Default);
		}

		void Configure(const Config&);

		void AsyncAddManager(int componentType, ComponentManager*);
		void AsyncRemoveManager(int componentType, const String&, bool deleteManager = true);
		void AsyncDeleteAll();


		ComponentManager *AsyncFindManager(int componentType, const String& name);

	protected:

		typedef map<String, ComponentManager*>::type ComponentManagerMap;

		NEX_THREAD_MUTEX(containerLock);
		ComponentManagerMap componentManagers;
	};

} /* namespace nextar */
#endif /* MANAGERROOT_H_ */
