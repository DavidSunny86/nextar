/*
 * ComponentManager.h
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */

#ifndef COMPONENTMANAGER_H_
#define COMPONENTMANAGER_H_

#include "Component.h"
#include "NexThread.h"

namespace nextar {

	class Component;
	class ComponentManagerArchive;
	class ComponentStreamer;

	/**
	 * @remarks ComponentManager is a container for components of a specific type.
	 * Component managers are themselves components and may be loaded and saved.
	 * Component managers are inherited by scene managers which acts as container of
	 * scene nodes.
	 */
	class ComponentManager: public NamedObject,
		public AllocComponent {
		NEX_LOG_HELPER(ComponentManager);

	public:
		enum Type {
			MANAGER_MESH, 
			MAGAGER_TEXTURE, 
			MANAGER_FONT, 
			MANAGER_SCENE
		};

		ComponentManager(const String&);
		virtual ~ComponentManager();

		virtual void Configure(const Config& config);

		virtual Component* AsyncCreate(int type, const String& name) = 0;
		virtual void AsyncDestroy(Component*) = 0;
		virtual Component* AsyncFind(const String& name) = 0;
		virtual Component* AsyncFindOrCreate(int type, const String& name) = 0;
		virtual void AsyncCleanup() = 0;
		
		static Component* AsyncFindComponent(int type, const String& comp);
		static Component* AsyncFindOrCreateComponent(int type, const String& comp);
		
	protected:

	};

	class ComponentManagerImpl: public ComponentManager {
		NEX_LOG_HELPER(ComponentManagerImpl);

	public:
		
		ComponentManagerImpl(const String& name);
		virtual ~ComponentManagerImpl();

		virtual Component* AsyncCreate(int type, const String& name);
		virtual void AsyncDestroy(Component*);
		virtual Component* AsyncFind(const String& name);
		virtual Component* AsyncFindOrCreate(int type, const String& name);
		virtual void AsyncCleanup();

	protected:

		virtual Component* AsyncCreateImpl(int type, const String& name) = 0;
		virtual void AsyncDestroyImpl(Component* component);
		
		NEX_THREAD_MUTEX(containerLock);
		ComponentMap componentMap;
	};

} /* namespace nextar */
#endif /* COMPONENTMANAGER_H_ */
