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
	typedef unordered_map<String, Component*>::type ComponentMap;

	/**
	 * @remarks Component is the base of all game components. Game components include:
	 * Scene nodes, meshes, textures, sounds, fonts, widgets etc.
	 * Components may depend on multiple components and will issue load/save calls using
	 * dependency order.
	 */
	class Component: public NamedObject, 
		public AllocComponent {
		NEX_LOG_HELPER(Component);
	public:

		/* known types */
		enum {
			COMPONENT_UNKNOWN = -1,
			COMPONENT_ASSET_MESH = 10010,
			COMPONENT_ASSET_SHADER,
			COMPONENT_ASSET_TEXTURE,
			COMPONENT_ASSET_MATERIAL,

			COMPONENT_RENDERABLE_MESH = 10110,
			COMPONENT_RENDERABLE_LIGHT,
			COMPONENT_MOVABLE,
		};

		enum Flags {
			ENABLED = 1 << 0,
			IS_NODE = 1 << 1,
			LAST_FLAG = 1 << 2,
		};

	public:

		Component(ComponentManager *creator = 0, const String & name = StringUtils::Null);
		virtual ~Component();

		inline ComponentManager *GetCreator() const {
			return creator;
		}

		inline bool IsNode() const {
			return (flags & IS_NODE) != 0;
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

		virtual int GetType() const = 0;
		virtual void SetEnabled(bool enabled);

		virtual Component* AsyncFindChild(const String& name);
	protected:

		uint32 flags;

		BoundingVolume *bounds;
		ComponentManager *creator;
	};

} /* namespace nextar */
#endif /* COMPONENT_H_ */
