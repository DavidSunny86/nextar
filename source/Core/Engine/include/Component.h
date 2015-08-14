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
 *
 * @todo
 * Need AttachTo and Detach methods. AttachTo will attach based on if this component
 * can be attached to the other component. Components may override this behaviour and
 * thus disallow attachment to specific components.
 */
class _NexEngineAPI Component: public NamedObject, public AllocComponent {
	NEX_LOG_HELPER(Component)
	;
public:
	// factory, object

	template <typename ComponentType>
	class ComponentTraits {
	public:
		enum {
			CLASS_ID = ComponentType::CLASS_ID
		};

		inline static uint32 GetClassID() {
			return CLASS_ID;
		}
		
		inline static uint32 GetCatagory() {
			return COMPONENT_CAT(GetClassID());
		}

		template <typename CastFrom>
		inline static ComponentType* Cast(CastFrom* anyComponent) {
			if(COMPONENT_CAT(anyComponent->GetClassID()) == COMPONENT_CAT(GetClassID()))
				return static_cast<ComponentType*>(anyComponent);
			return nullptr;
		}
	};

	struct ID {
		StringID name;
		StringID factory;

		ID(StringID _name, StringID _factory) : name(_name), factory(_factory) {}
		ID(StringID _name) : name(_name), factory(StringUtils::DefaultID) {}
		ID() : name(StringUtils::NullID), factory(StringUtils::DefaultID) {}

		inline friend OutputSerializer& operator << (OutputSerializer& ser, const Component::ID& id) {
			ser << id.name << id.factory;
			return ser;
		}
		inline friend InputSerializer& operator >> (InputSerializer& ser, Component::ID& id) {
			ser >> id.name >> id.factory;
			return ser;
		}
		inline friend bool operator == (const ID& one, const ID& other) {
			return one.name == other.name && one.factory == other.factory;
		}
		inline friend bool operator != (const ID& one, const ID& other) {
			return !(one==other);
		}
	};

	static ID NullID;
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
		CAT_TEMPLATE = 1 << 10,
		CAT_BEHAVIOR = 1 << 11,
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

		CLASS_FPS_BEHAVIOR = COMPONENT_CLASS_ID(CAT_BEHAVIOR, 0),
		// templates
		CLASS_SHADER_TEMPLATE = COMPONENT_CLASS_ID(CAT_ASSET | CAT_TEMPLATE, 0),
		CLASS_MATERIAL_TEMPLATE = COMPONENT_CLASS_ID(CAT_ASSET | CAT_TEMPLATE, 1),
		CLASS_MESH_TEMPLATE = COMPONENT_CLASS_ID(CAT_ASSET | CAT_TEMPLATE, 2),
	};

	enum Flags {
		ENABLED = 1 << 0,
		UPDATE_REQUIRED = 1 << 1,
		LAST_FLAG = 1 << 2,
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

	Component(const StringID name = StringUtils::NullID,
			const StringID factory = StringUtils::DefaultID,
			Component* parent = nullptr);
	virtual ~Component();

	static Component* Instance(uint32 classId, StringID name,
			Component::Factory* factory = nullptr);
	// factory:group.name
	static ID ParseID(const String&);

	inline bool IsEnabled() const {
		return (flags & ENABLED) != 0;
	}

	inline void SetFactory(StringID factoy) {
		this->factory = factory;
	}

	inline StringID GetFactory() const {
		return factory;
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

	inline void SetUpdateRequired(bool v) {
		SetFlag(UPDATE_REQUIRED, v);
	}

	inline Component* GetParent() const {
		return parent;
	}

	// get the connected entity
	Entity* GetEntity();

	inline uint32 GetComponentCatagory() const {
		return COMPONENT_CAT(GetClassID());
	}

	inline bool IsUpdateRequired() const {
		return IsFlagSet(UPDATE_REQUIRED);
	}

	inline StringID GetID() const {
		return NamedObject::GetID();
	}

	inline void GetID(ID& id) const {
		id.name = NamedObject::GetID();
		id.factory = factory;
	}

	virtual void Update(const FrameTimer& frameTimer) {}
	// @remarks Concrete classes implement this
	virtual uint32 GetClassID() const = 0;

	virtual void SetParent(Component*);
	virtual void SetEnabled(bool enabled);

	virtual Component* FindComponent(const StringID name);

	static uint32 GetComponentCatagory(uint32 classID) {
		return COMPONENT_CAT(classID);
	}

protected:

	StringID factory;
	uint32 flags;
	// parent component this component is attached to
	Component* parent;
};

class _NexEngineAPI SharedComponent: public Referenced<SharedComponent,
		Component> {

public:
	// factory, group, object
	struct ID : public Component::ID {
		StringID group;

		ID(StringID _name, StringID _factory, StringID _group) : Component::ID(_name, _factory), group(_group) {}
		ID(StringID _name, StringID _factory) : Component::ID(_name, _factory), group(StringUtils::DefaultID) {}
		ID(StringID _name) : Component::ID(_name), group(StringUtils::DefaultID) {}
		ID() : Component::ID(), group(StringUtils::DefaultID) {}

		inline friend OutputSerializer& operator << (OutputSerializer& ser, const SharedComponent::ID& id) {
			ser << (*(const Component::ID*)&id) << id.group;
			return ser;
		}

		inline friend InputSerializer& operator >> (InputSerializer& ser, SharedComponent::ID& id) {
			ser >> (*(Component::ID*)&id) >> id.group;
			return ser;
		}

		inline friend bool operator == (const ID& one, const ID& other) {
			if ( (*(const Component::ID*)&one) == (*(const Component::ID*)&other)
					&& (one.group == other.group))
				return true;
			return false;
		}

		inline friend bool operator != (const ID& one, const ID& other) {
			return !(one==other);
		}
	};

	static ID NullID;
	static SharedComponentPtr Null;

	enum InstanceResult : uint8 {
		INSTANCE_CREATED,
		INSTANCE_RETRIEVED,
		INSTANCE_FAILED,
	};

	class _NexEngineAPI Group: public AllocGeneral, public NamedObject {
	public:

		class Lock {
		public:
			Lock(Group* group) : _group(group) {
				if(_group) _group->AcquireLock();
			}
			~Lock() {
				if(_group) _group->ReleaseLock();
			}
		protected:
			Group* _group;
		};

		Group(const StringID name);
		virtual ~Group();
		virtual void AcquireLock() = 0;
		virtual void ReleaseLock() = 0;
		virtual void Add(SharedComponentPtr&) = 0;
		virtual SharedComponentPtr& Find(uint32 classId, const StringID name) = 0;
		virtual void Remove(uint32 classId, StringID name) = 0;
		virtual void Remove(StringID name) = 0;

		virtual void AsyncRemoveAll(
				uint32 ofType = Component::CLASS_UNKNOWN) = 0;
		virtual void AsyncCollect(Group* container, uint32 ofType) = 0;
	};

	SharedComponent(const StringID name,
			const StringID factory,
			Component* parent = nullptr, Group* addToGroup = nullptr);
	virtual ~SharedComponent();

	/** Creates or gets an instance of an existing resource of the name specified
	 * from the group repository (when stored). Returns InstanceResult based on
	 * the outcome */
	static InstanceResult Instance(SharedComponentPtr& oInst, uint32 classId, StringID name,
			Component::Factory* factory = nullptr,
			SharedComponent::Group* group = nullptr);
	static InstanceResult Instance(SharedComponentPtr& oInst, uint32 classId, StringID name,
			StringID factory = StringUtils::DefaultID,
			SharedComponent::Group* group = nullptr);
	// factory:group.name
	static ID ToID(const String&);

	inline Group* GetGroup() const {
		return group;
	}

	inline StringID GetID() const {
		return Component::GetID();
	}

	inline void GetID(Component::ID& id) const {
		Component::GetID(id);
	}

	inline void GetID(ID& id) const {
		Component::GetID(id);
		id.group = group ? group->GetID() : StringUtils::NullID;
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
