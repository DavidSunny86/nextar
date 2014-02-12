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

		virtual Component* AsyncCreate(uint32 classId, const String& name) = 0;
		virtual void AsyncDestroy(Component*) = 0;
		virtual Component* AsyncFind(const String& name) = 0;
		virtual Component* AsyncFindOrCreate(uint32 classId, const String& name) = 0;
		virtual void AsyncCleanup() = 0;
		
		static Component* AsyncFindComponent(uint32 classId, const String& comp);
		static Component* AsyncFindOrCreateComponent(uint32 classId, const String& comp);
		
	protected:

	};

	class ComponentManagerImpl: public ComponentManager {
		NEX_LOG_HELPER(ComponentManagerImpl);

	public:
		
		ComponentManagerImpl(const String& name);
		virtual ~ComponentManagerImpl();

		virtual Component* AsyncCreate(uint32 classId, const String& name) override;
		virtual void AsyncDestroy(Component*) override;
		virtual Component* AsyncFind(const String& name) override;
		virtual Component* AsyncFindOrCreate(uint32 classId, const String& name) override;
		virtual void AsyncCleanup() override;

	protected:

		virtual Component* AsyncCreateImpl(uint32 classId, const String& name) = 0;
		virtual void AsyncDestroyImpl(Component* component);
		
		NEX_THREAD_MUTEX(containerLock);
		ComponentMap componentMap;
	};

} /* namespace nextar */
#endif /* COMPONENTMANAGER_H_ */
