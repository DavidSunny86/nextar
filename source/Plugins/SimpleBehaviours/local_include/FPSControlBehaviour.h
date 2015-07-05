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
		public nextar::Behaviour,
		public nextar::InputListener {
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
	virtual void ProcessEvents(const InputChangeBuffer&);

	static void Populate(PropertyDictionary*);
	static void RemoveDictionary();

protected:

	float movementSpeed;
	float rotationSpeed;
	Vector3A* deltaStrafe;
	Vector3A* deltaForward;
	Quaternion* deltaRotation;
};

} /* namespace SimpleBehaviours */

#endif /* PLUGINS_SIMPLEBEHAVIOURS_LOCAL_INCLUDE_FPSCONTROLBEHAVIOUR_H_ */
