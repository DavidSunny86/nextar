/*
 * CullingSystem.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef CULLINGSYSTEM_H_
#define CULLINGSYSTEM_H_

#include "NexSys.h"

namespace nextar {

	class CullingSystem {
	public:
		CullingSystem();
		virtual ~CullingSystem();

		virtual void NotifyNodeAdded(Movable*) {}
		virtual void NotifyNodeRemoved(Movable*) {}
	};

} /* namespace nextar */
#endif /* CULLINGSYSTEM_H_ */
