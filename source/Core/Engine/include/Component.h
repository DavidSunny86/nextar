/*
 * Component.h
 *
 *  Created on: Oct 3, 2011
 *      Author: obhi
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <NexEngine.h>
#include <BoundingVolume.h>
#include <StringUtils.h>
#include <NamedObject.h>

namespace nextar {

#define COMPONENT_CLASS_ID(catagory, typenum)	(uint32((catagory) << 10) | uint32(((uint16)typenum)))
#define COMPONENT_CAT(fromClassID) ((uint32)((fromClassID) >> 10) & 0x003fffff)
//#define COMPONENT_TYPE(fromClassID) ((uint32)((fromClassID)) & 0x000003ff)
/**
 * @remarks Component is the base of all game components. Game components include:
 * Scene nodes, meshes, textures, sounds, fonts, widgets etc.
 * Components may depend on multiple components and will issue load/save calls using
 * dependency order.
 */
class _NexEngineAPI Component: public NamedObject, public AllocComponent {
	NEX_LOG_HELPER(Component)
	;
public:

	/* catagory the component belongs to aka Generalization */
	enum Catagory {
		CAT_ASSET = 1 << 0,
		CAT_ENTITY = 1 << 1,
		CAT_RENDERABLE = 1 << 2,
		CAT_MOVEABLE = 1 << 3,
		CAT_LIGHT = 1 << 4,
		CAT_CAMERA = 1 << 5,
		CAT_SCENE = 1 << 6,
		CAT_CULLING_SYSTEM = 1 << 8,
		CAT_SPATIAL = 1 << 9,
	};

	/* specific components implemented in the system */
	enum ComponentClasses {
		CLASS_UNKNOWN = COMPONENT_CLASS_ID(0, 0),
		CLASS_ASSET_MESH = COMPONENT_CLASS_ID(CAT_ASSET, 0),
		CLASS_ASSET_SHADER = COMPONENT_CLASS_ID(CAT_ASSET, 1),
		CLASS_ASSET_TEXTURE = COMPONENT_CLASS_ID(CAT_ASSET, 2),
		CLASS_ASSET_MATERIAL = COMPONENT_CLASS_ID(CAT_ASSET, 3),

		CLASS_ENTITY = COMPONENT_CLASS_ID(CAT_ENTITY, 0),

		CLASS_MESH = COMPONENT_CLASS_ID(CAT_SPATIAL | CAT_RENDERABLE, 0),
		CLASS_LIGHT = COMPONENT_CLASS_ID(
				CAT_SPATIAL | CAT_RENDERABLE | CAT_LIGHT, 0),
		CLASS_MOVABLE = COMPONENT_CLASS_ID(CAT_MOVEABLE, 0),
		CLASS_CAMERA = COMPONENT_CLASS_ID(CAT_SPATIAL | CAT_CAMERA, 0),

		CLASS_SCENE = COMPONENT_CLASS_ID(CAT_ASSET | CAT_SCENE, 0),
		CLASS_BV_CULLER = COMPONENT_CLASS_ID(CAT_SPATIAL | CAT_CULLING_SYSTEM,
				0),
	};

	enum Flags {
		ENABLED = 1 << 0, LAST_FLAG = 1 << 1,
	};

	class Factory;
	/* */
	class _NexEngineAPI Factory: public AllocGeneral,
		public NamedObject {
	public:
		Factory(const StringID);
		virtual ~Factory();
		virtual void Configure(const Config&) {
		}
		virtual Component* AsyncCreate(uint32 classId, const StringID name) = 0;
	};

public:

	Component(const StringID name = StringUtils::NullID, Component* parent =
			nullptr);
	virtual ~Component();

	static Component* Instance(uint32 classId, StringID name,
			Component::Factory* factory = nullptr);

	inline bool IsEnabled() const {
		return (flags & ENABLED) != 0;
	}

	inline void SetFlag(uint32 flag, bool v) {
		flags = v ? flag | flags : ~flag & flags;
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

	inline uint32 GetComponentCatagory() const {
		return COMPONENT_CAT(GetClassID());
	}

	virtual uint32 GetClassID() const = 0;

	virtual void SetParent(Component*);
	virtual void SetEnabled(bool enabled);

	virtual Component* FindComponent(const StringID name);

	static uint32 GetComponentCatagory(uint32 classID) {
		return COMPONENT_CAT(classID);
	}

protected:

	uint32 flags;
	// parent component this component is attached to
	Component* parent;
};

class _NexEngineAPI SharedComponent: public Referenced<SharedComponent,
		Component> {

public:
	static SharedComponentPtr Null;

	class _NexEngineAPI Group: public AllocGeneral, public NamedObject {
	public:

		Group(const StringID name);
		virtual ~Group();
		virtual void AsyncAdd(SharedComponentPtr&) = 0;
		virtual SharedComponentPtr& AsyncFind(const StringID name) = 0;
		virtual void AsyncRemove(StringID name) = 0;
		virtual void AsyncRemoveAll(
				uint32 ofType = Component::CLASS_UNKNOWN) = 0;
		virtual void AsyncCollect(Group* container, uint32 ofType) = 0;
	};

	SharedComponent(const StringID name = StringUtils::NullID,
			Component* parent = nullptr, Group* addToGroup = nullptr);
	virtual ~SharedComponent();

	static SharedComponentPtr Instance(uint32 classId, StringID name,
			Component::Factory* factory = nullptr,
			SharedComponent::Group* group = nullptr);

	inline Group* GetGroup() const {
		return group;
	}

	/** Components can only belong to a single group */
	void AddToGroup(Group *g);
	/** Detach from the current group */
	void RemoveFromGroup();
protected:
	Group *group;
};

} /* namespace nextar */
#endif /* COMPONENT_H_ */
