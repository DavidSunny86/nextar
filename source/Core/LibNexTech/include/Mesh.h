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

	class Mesh: public nextar::Renderable {
	public:

		enum {
			TYPE = TYPE_MESH,
		};

		struct Primitive {
			uint8 layer;
			uint8 flags;
			uint32 sortKey;
			VisiblePrimitive primitive;
		};

		Mesh(ComponentManager* creator, const String& name);
		virtual ~Mesh();

		/** @brief Get node type */
		virtual int GetComponentType() const;
		virtual int GetComponentCatagory() const;

		virtual void RegisterVisibles(SceneTraversal& traversal);

	protected:
	};

} /* namespace nextar */
#endif /* RENDERABLEMESH_H_ */
