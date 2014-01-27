/*
 * RenderableMesh.h
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#ifndef RENDERABLEMESH_H_
#define RENDERABLEMESH_H_

#include <Renderable.h>

namespace nextar {

	class RenderableMesh: public nextar::Renderable {
	public:

		struct Primitive {
			uint8 layer;
			uint8 flags;
			uint32 sortKey;
			VisiblePrimitive primitive;
		};

		RenderableMesh();
		virtual ~RenderableMesh();


		/** @brief Get node type */
		virtual int GetType() const;

		virtual void RegisterVisibles(SceneTraversal& traversal);
	};

} /* namespace nextar */
#endif /* RENDERABLEMESH_H_ */
