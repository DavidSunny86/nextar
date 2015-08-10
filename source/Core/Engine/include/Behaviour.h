/*
 * Behavior.h
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#ifndef CORE_ENGINE_INCLUDE_BEHAVIOUR_H_
#define CORE_ENGINE_INCLUDE_BEHAVIOUR_H_

#include <Component.h>
#include <PropertyInterface.h>

namespace nextar {

/**
 * @remarks The idea of behaviors is that, they are invoked by
 * events or triggers. Behaviors can define multiple actions, any
 * of which can be triggered based on an event.
 */
class _NexEngineAPI Behaviour: public PropertyInterface,
	public Component {

public:

	Behaviour(const StringID name = StringUtils::NullID,
				const StringID factory = StringUtils::DefaultID,
				Component* parent = nullptr);

	enum {
		CATAGORY = CAT_BEHAVIOR,
	};

	virtual uint32 GetClassID() const = 0 ;
	virtual void Update(const FrameTimer& frameTimer) override {}
};

} /* namespace nextar */

#endif /* CORE_ENGINE_INCLUDE_BEHAVIOUR_H_ */
