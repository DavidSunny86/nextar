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

FPSControlBehaviour::ControllerDeviceAccessor FPSControlBehaviour::ControllerDeviceAccessor::_accessor;
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
Behaviour(name, factory), controls(nullptr),
movementSpeedFactor(50.1f), rotationSpeedFactor(1.5f) {

	if (OverrideDictionary("ControllerProperties")) {
		Populate(GetDictionary());
	}

}

FPSControlBehaviour::~FPSControlBehaviour() {
	if (controls)
		NEX_DELETE(controls);
}

void FPSControlBehaviour::Populate(PropertyDictionary* dic) {
	dic->AddParam("device_id", &FPSControlBehaviour::ControllerDeviceAccessor::_accessor,
			PropertyType::PROP_TYPE_UINT);
	// @todo Params for movement_speed_factor, rotate_speed_factor
}

uint32 FPSControlBehaviour::GetClassID() const {
	return CLASS_ID;
}

void FPSControlBehaviour::Update(const FrameTimer& frameTimer) {
	Entity* attached = GetEntity();
	if (attached) {
		Moveable* m = attached->GetMoveable();
		if (m && controls) {
			float timeDelta = frameTimer.GetFrameTime();
			controls->Process(m, timeDelta, movementSpeedFactor, rotationSpeedFactor);
		}
	}
}

void FPSControlBehaviour::SetControllerID(uint32 deviceId) {
	const InputControllerDesc& desc = InputManager::Instance().GetControllerDescById(deviceId);
	if (desc.IsValid()) {
		if (desc.type == ControllerType::TYPE_XBOX360_CONTROLLER)
			controls = NEX_NEW(XBox360Controller());
		else if (desc.type == ControllerType::TYPE_KEYBOARD_AND_MOUSE) {
			controls = NEX_NEW(KeyboardMouseController());
			InputManager::Instance().SetProperty(deviceId, "lock_cursor", "true");
		}
		InputManager::Instance().RegisterListener(controls, deviceId);
	}
}

void FPSControlBehaviour::RemoveDictionary() {
	PropertyInterface::RemoveDictionary("ControllerProperties");
}

void FPSControlBehaviour::XBox360Controller::Process(Moveable* m,
	float timeDelta,
	float movementSpeedFactor, float rotationSpeedFactor) {
	// @todo We need the timer here
	Vector2 movement(movementSpeedFactor * moveSpeedXZ.x * timeDelta,
		-movementSpeedFactor * moveSpeedXZ.y * timeDelta);
	Vector2 rotation(rotationSpeedFactor * rotateSpeedXY.x * timeDelta,
		rotationSpeedFactor * rotateSpeedXY.y * timeDelta);
	m->LocalApplyCameraMotion(movement, rotation);
}

void FPSControlBehaviour::XBox360Controller::ProcessEvents(const InputChangeBuffer& events) {
	float strafeX = 0;
	float strafeZ = 0;

	for (uint32 i = 0; i < events.second; ++i) {
		const InputEvent& ie = events.first[i];

		switch (ie.key)	{
			// control camera movement
		case KeyName::XBOX_AXIS_LEFT:
			moveSpeedXZ.x = ie.analogDir.xy[0];
			moveSpeedXZ.y = ie.analogDir.xy[1];
			break;
			// control camera look-at
		case KeyName::XBOX_AXIS_RIGHT:
			rotateSpeedXY.x = -ie.analogDir.xy[1];
			rotateSpeedXY.y = ie.analogDir.xy[0];
			break;
		}
	}
}

void FPSControlBehaviour::KeyboardMouseController::ProcessEvents(const InputChangeBuffer& events) {
	float strafeX = 0;
	float strafeZ = 0;

	for (uint32 i = 0; i < events.second; ++i) {
		const InputEvent& ie = events.first[i];

		switch (ie.key)	{
			// control camera movement
		case KeyName::KB_W:
			moveSpeedXZ.y = (ie.keyState == KeyState::KEY_STATE_DOWN) ? 1.0f : 0.0f;
			break;
		case KeyName::KB_S:
			moveSpeedXZ.y = (ie.keyState == KeyState::KEY_STATE_DOWN) ? -1.0f : 0.0f;
			break;
		case KeyName::KB_A:
			moveSpeedXZ.x = (ie.keyState == KeyState::KEY_STATE_DOWN) ? -1.0f : 0.0f;
			break;
		case KeyName::KB_D:
			moveSpeedXZ.x = (ie.keyState == KeyState::KEY_STATE_DOWN) ? 1.0f : 0.0f;
			break;
			// control camera look-at
		case KeyName::MOUSE_XY_AXIS:
			rotateSpeedXY.x -= ie.analogDir.xy[1] * 0.1f;
			rotateSpeedXY.y -= ie.analogDir.xy[0] * 0.1f;
			break;
		}
	}
}

void FPSControlBehaviour::KeyboardMouseController::Process(Moveable* m,
	float timeDelta,
	float movementSpeedFactor, float rotationSpeedFactor) {
	// @todo We need the timer here
	Vector2 movement(movementSpeedFactor * moveSpeedXZ.x * timeDelta,
		-movementSpeedFactor * moveSpeedXZ.y * timeDelta);
	Vector2 rotation(rotationSpeedFactor * rotateSpeedXY.x * timeDelta,
		rotationSpeedFactor * rotateSpeedXY.y * timeDelta);
	m->LocalApplyCameraMotion(movement, rotation);
	rotateSpeedXY.x = 0;
	rotateSpeedXY.y = 0;
}


} /* namespace SimpleBehaviours */
