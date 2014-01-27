/*
 * ComponentLoader.h
 *
 *  Created on: 14-Oct-2013
 *      Author: obhi
 */

#ifndef COMPONENTLOADER_H_
#define COMPONENTLOADER_H_

#include "NexTypes.h"
#include "Component.h"

namespace nextar {

	class ComponentLoader : public Singleton<ComponentLoader>,
							public AllocScene {
	public:

		/* Add a component for loading */
		virtual void RequestLoad(Component*);
		virtual void RequestReady(Component*);

		ComponentLoader();
		virtual ~ComponentLoader();
	};

} /* namespace nextar */
#endif /* COMPONENTLOADER_H_ */
