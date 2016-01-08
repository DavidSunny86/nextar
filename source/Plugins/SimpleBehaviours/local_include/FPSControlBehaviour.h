/*
 * FPSControlBehaviour.h
 *
 *  Created on: 24-May-2015
 *      Author: obhi
 */

#ifndef PLUGINS_SIMPLEBEHAVIOURS_LOCAL_INCLUDE_FPSCONTROLBEHAVIOUR_H_
#define PLUGINS_SIMPLEBEHAVIOURS_LOCAL_INCLUDE_FPSCONTROLBEHAVIOUR_H_

#include <SimpleBehaviours.h>
#include <Behaviour.h>
#include <InputController.h>

using namespace nextar;
namespace SimpleBehaviours {

class FPSControlBehaviour:
		public nextar::Behaviour {
	
	class ControlMap : public nextar::InputListener,
		public AllocGeneral {
	public:
		
		Vector2 moveSpeedXZ;
		Vector2 rotateSpeedXY;

		ControlMap() : moveSpeedXZ(0, 0), rotateSpeedXY(0, 0) { }

		void Reset() {
			moveSpeedXZ.Set(0, 0);
			rotateSpeedXY.Set(0, 0);
		}

		virtual ~ControlMap() {}
		virtual void Process(Moveable* m, float timeDelta, float moveSpeed, float rotateSpeed) = 0;
	};

	class XBox360Controller : public ControlMap {
	public:
		virtual void ProcessEvents(const InputChangeBuffer&);
		virtual void Process(Moveable* m, float timeDelta, float moveSpeed, float rotateSpeed);
	};

	class KeyboardMouseController : public ControlMap {
	public:
		KeyboardMouseController() {
		}
		virtual void ProcessEvents(const InputChangeBuffer&);
		virtual void Process(Moveable* m, float timeDelta, float moveSpeed, float rotateSpeed);
	};

public:
	
	typedef ComponentTraits<FPSControlBehaviour> Traits;
	enum {
		CLASS_ID = Component::CLASS_FPS_BEHAVIOR,
		CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	enum Controller {
		TYPE_XBOX360,
		TYPE_KEYBOARD_MOUSE,
	};

	class Factory: public Component::Factory {
		NEX_LOG_HELPER(FPSControlBehaviourFactory);
	public:
		Factory(const StringID name);

		/* implementation */
		virtual Component* AsyncCreate(uint32 classId, const StringID name);
	};

	class ControllerDeviceAccessor : public PropertyAccessor {
	public:
		static ControllerDeviceAccessor _accessor;
		virtual void SetStringValue(PropertyInterface* intf,
					const String& value);
		virtual const String GetStringValue(
					const PropertyInterface* intf) const;
	};

	FPSControlBehaviour(const StringID name, const StringID factory);
	virtual ~FPSControlBehaviour();

	void SetControllerID(uint32 id);

	virtual uint32 GetClassID() const;
	virtual void Update(const FrameTimer& frameTimer) override;
	
	static void Populate(PropertyDictionary*);
	static void RemoveDictionary();

protected:

	float movementSpeedFactor;
	float rotationSpeedFactor;
	ControlMap* controls;
};

} /* namespace SimpleBehaviours */

#endif /* PLUGINS_SIMPLEBEHAVIOURS_LOCAL_INCLUDE_FPSCONTROLBEHAVIOUR_H_ */
