/*
 * Behavior.h
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#ifndef CORE_ENGINE_INCLUDE_BEHAVIOR_H_
#define CORE_ENGINE_INCLUDE_BEHAVIOR_H_

#include <Component.h>

namespace nextar {

/**
 * @remarks The idea of behaviors is that, they are invoked by
 * events or triggers. Behaviors can define multiple actions, any
 * of which can be triggered based on an event.
 */
class Behavior: public Component {

public:

	virtual uint32 GetClassID() const = 0 ;
	virtual void Update() override {}
};

} /* namespace nextar */

#endif /* CORE_ENGINE_INCLUDE_BEHAVIOR_H_ */
