/*
 * Renderable.h
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include <Spatial.h>
#include <VisiblePrimitive.h>

namespace nextar {

	class Renderable: public nextar::Spatial {
	public:
		enum {
			CATAGORY = Component::CAT_RENDERABLE
		};
		
		Renderable(const StringID name, Component* parent = nullptr);
		virtual ~Renderable();

		virtual uint32 GetClassID() const override = 0;
		virtual void Visit(SceneTraversal& traversal) override = 0;

	protected:
	};

} /* namespace nextar */
#endif /* RENDERABLE_H_ */
