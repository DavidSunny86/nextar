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

namespace SimpleBehaviours {

class FPSControlBehaviour:
		public nextar::Behaviour,
		public nextar::InputListener {
public:
	enum Controller {
		TYPE_XBOX360,
		TYPE_KEYBOARD_MOUSE,
	};

	FPSControlBehaviour(uint16 deviceId);
	virtual ~FPSControlBehaviour();

	virtual uint32 GetClassID() const;
	virtual void Update() override;

	virtual void ProcessEvents(const InputChangeBuffer&);

protected:

};

} /* namespace SimpleBehaviours */

#endif /* PLUGINS_SIMPLEBEHAVIOURS_LOCAL_INCLUDE_FPSCONTROLBEHAVIOUR_H_ */
