/*
 * Entity.h
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include "Component.h"

namespace nextar {

	class Entity: public nextar::Component {
	public:
		Entity();
		virtual ~Entity();

		/** @brief Visit renderables attached to this node **/
		virtual void FindVisibles(SceneTraversal& traversal) = 0;

	protected:

		Movable* moveable;
		Renderable* renderable;
		RenderableLight* light;
	};

} /* namespace nextar */
#endif /* ENTITY_H_ */
