/*
 * FPSControlBehaviour.cpp
 *
 *  Created on: 24-May-2015
 *      Author: obhi
 */

#include <InputManager.h>
#include <FPSControlBehaviour.h>
#include <Moveable.h>

namespace SimpleBehaviours {

/******* Factory ***********/
FPSControlBehaviour::Factory::Factory(const StringID name) : Component::Factory(name) {
}

Component* FPSControlBehaviour::Factory::AsyncCreate(uint32 classId, const StringID name) {
	if (classId == Component::CLASS_FPS_BEHAVIOR) {
		return NEX_NEW(FPSControlBehaviour(name, GetID()));
	}
	return nullptr;
}

/******* Factory ***********/
/******* Property Accessors ***********/

void FPSControlBehaviour::ControllerDeviceAccessor::SetStringValue(
		PropertyInterface* intf, const String& value) {
	static_cast<FPSControlBehaviour*>(intf)->SetControllerID(Convert::ToULong(value));
}

const String FPSControlBehaviour::ControllerDeviceAccessor::GetStringValue(
		const PropertyInterface* intf) const {
	//@todo
	return StringUtils::Null;
}

/******* Property Accessors ***********/
FPSControlBehaviour::FPSControlBehaviour(const StringID name, const StringID factory) :
	Behaviour(name, factory),
	movementSpeed(2.0f), rotationSpeed(0.1f) {

	if (OverrideDictionary("ControllerProperties")) {
		Populate(GetDictionary());
	}
	deltaStrafe = NEX_ALLOCATOR_ALLOC_T(Vector3A, AllocatorVector3A);
	deltaForward = NEX_ALLOCATOR_ALLOC_T(Vector3A, AllocatorVector3A);
	deltaRotation = NEX_ALLOCATOR_ALLOC_T(Quaternion, AllocatorQuaternion);
}

FPSControlBehaviour::~FPSControlBehaviour() {
	NEX_ALLOCATOR_FREE_T(Vector3A, AllocatorVector3A, deltaStrafe);
	NEX_ALLOCATOR_FREE_T(Vector3A, AllocatorVector3A, deltaStrafe);
	NEX_ALLOCATOR_FREE_T(Quaternion, AllocatorVector3A, deltaRotation);
}

void FPSControlBehaviour::Populate(PropertyDictionary* dic) {
	dic->AddParam("device_id", &FPSControlBehaviour::ControllerDeviceAccessor::_accessor,
			PropertyType::PROP_TYPE_UINT);
}

uint32 FPSControlBehaviour::GetClassID() const {
	return CLASS_ID;
}

void FPSControlBehaviour::Update(const FrameTimer& frameTimer) {
	Entity* attached = GetEntity();
	if (attached) {
		Moveable* m = attached->GetMoveable();
		if (m) {
			// @todo We need the timer here
			Vector3A v = Vec3AMulScalar(*deltaStrafe, movementSpeed*frameTimer.GetElapsedTime());
			m->TranslateBy(v);
		}
	}
}

void FPSControlBehaviour::ProcessEvents(const InputChangeBuffer& events) {
	float strafeX = 0;
	float strafeZ = 0;

	for(uint32 i = 0; i < events.second; ++i) {
		const InputEvent& ie = events.first[i];

		switch(ie.key)	{
		// control camera movement
		case Key::XBOX_AXIS_LEFT:
			if(ie.analogDir.xy[0] < 0)
				strafeX =-movementSpeed;
			else if (ie.analogDir.xy[0] > 0)
				strafeX = movementSpeed;
			if(ie.analogDir.xy[1] < 0)
				strafeZ =-movementSpeed;
			else if (ie.analogDir.xy[1] > 0)
				strafeZ = movementSpeed;
			break;
		// control camera look-at
		case Key::XBOX_AXIS_RIGHT:
			break;
		}
	}
	*deltaStrafe = Vec3ASet(strafeX, 0, strafeZ);
}

void FPSControlBehaviour::SetControllerID(uint32 deviceId) {
	InputManager::Instance().RegisterListener(this, deviceId);
}

void FPSControlBehaviour::RemoveDictionary() {
	PropertyInterface::RemoveDictionary("ControllerProperties");
}

} /* namespace SimpleBehaviours */
