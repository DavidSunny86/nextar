/*
 * Renderable.h
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include <Component.h>

namespace nextar {

	class Renderable: public nextar::Component {
	public:
		Renderable();
		virtual ~Renderable();

		/** @brief Get node type */
		virtual int GetType() const;

		virtual void RegisterVisibles(SceneTraversal& traversal) = 0;

	protected:
	};

} /* namespace nextar */
#endif /* RENDERABLE_H_ */
