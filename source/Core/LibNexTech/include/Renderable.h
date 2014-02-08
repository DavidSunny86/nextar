/*
 * Renderable.h
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include <Spatial.h>

namespace nextar {

	class Renderable: public nextar::Spatial {
	public:
		enum {
			CATAGORY = Component::CAT_RENDERABLE
		};
		
		Renderable(ComponentManager* creator, const String& name, Component* parent);
		virtual ~Renderable();

		virtual uint32 GetClassID() const override = 0;
		virtual void Visit(SceneTraversal& traversal) override = 0;

	protected:
	};

} /* namespace nextar */
#endif /* RENDERABLE_H_ */
