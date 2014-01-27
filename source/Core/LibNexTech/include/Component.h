/*
 * Component.h
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "NexSys.h"
#include "BoundingVolume.h"
#include "StringUtils.h"
#include "Camera.h"
#include "NamedObject.h"

namespace nextar {


	typedef list<Component*>::type ComponentList;
	typedef vector<Component*>::type ComponentArray;
	typedef set<Component*>::type ComponentSet;
	typedef unordered_map<StringID, Component*>::type ComponentMap;

	/**
	 * @remarks Component is the base of all game components. Game components include:
	 * Scene nodes, meshes, textures, sounds, fonts, widgets etc.
	 * Components may depend on multiple components and will issue load/save calls using
	 * dependency order.
	 */
	class _NexExport Component: public NamedObject,
		public AllocComponent {
		NEX_LOG_HELPER(Component);
	public:

		/* known types */
		enum {
			COMPONENT_UNKNOWN = -1,
			COMPONENT_ASSET_MESH = 100,
			COMPONENT_ASSET_SHADER,
			COMPONENT_ASSET_TEXTURE,
			COMPONENT_ASSET_MATERIAL,

			COMPONENT_ENTITY,
			COMPONENT_MESH,
			COMPONENT_LIGHT,
			COMPONENT_MOVABLE,
			COMPONENT_CAMERA,
		};

		enum Flags {
			ENABLED = 1 << 0,
			LAST_FLAG = 1 << 2,
		};

	public:

		Component(ComponentManager *creator = nullptr, const String & name = StringUtils::Null, Entity* entity = nullptr);
		virtual ~Component();

		inline ComponentManager *GetCreator() const {
			return creator;
		}

		inline BoundingVolume *GetBounds() const {
			return bounds;
		}

		inline void SetCreator(ComponentManager *creator) {
			this->creator = creator;
		}

		inline void SetBounds(BoundingVolume *bounds) {
			this->bounds = bounds;
		}

		inline bool IsEnabled() const {
			return (flags & ENABLED) != 0;
		}

		inline void SetFlag(uint32 flag, bool v) {
			flags = v? flag | flags : ~flag & flags;
		}

		inline void SetFlag(uint32 flag) {
			flags |= flag;
		}

		inline void UnsetFlag(uint32 flag) {
			flags &= ~flag;
		}

		inline void SetFlags(uint32 flags) {
			this->flags = flags;
		}

		inline uint32 GetFlags() const {
			return flags;
		}

		inline bool IsFlagSet(uint32 f) const {
			return (flags & f) != 0;
		}

		inline Component* GetParent() const {
			return parent;
		}

		virtual int GetComponentType() const = 0;
		virtual void SetEnabled(bool enabled);

		inline Component* AsyncFindChild(const String& name) {
			return AsyncFindChild(nameTable.AsyncStringID(name));
		}

		virtual Component* AsyncFindChild(const StringID name);

	protected:

		uint32 flags;

		BoundingVolume *bounds;
		ComponentManager *creator;
		// parent component this component is attached to
		Component* parent;
	};

} /* namespace nextar */
#endif /* COMPONENT_H_ */
