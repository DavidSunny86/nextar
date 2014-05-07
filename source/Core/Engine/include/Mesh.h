/*
 * RenderableMesh.h
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */

#ifndef MESH_H_
#define MESH_H_

#include <Renderable.h>

namespace nextar {

	class Mesh: public nextar::Renderable {
	public:
		
		enum Type {
			CLASS_ID = Component::CLASS_MESH,
			CATAGORY = COMPONENT_CAT(CLASS_ID),
		};

		struct Primitive {
			uint8 flags;
			uint8 layer;
			uint16 renderQueue;
			uint32 sortKey;
			VisiblePrimitive primitive;

			~Primitive();
		};

		Mesh(const StringID name, Component* parent = nullptr);
		virtual ~Mesh();

		/* recreate the object with new mesh asset */
		virtual void SetMeshAsset(MeshAssetPtr& asset);
		/** @brief Get node type */
		virtual uint32 GetClassID() const override;
		virtual void Visit(SceneTraversal& traversal) override;

		virtual void SetMaterial(uint32 primitive, MaterialAssetPtr& material);
	protected:
		typedef vector<Primitive>::type PrimitiveList;

		uint16 defaultRenderQueue;
		PrimitiveList primitives;
		MeshAssetPtr meshAsset;
	};

} /* namespace nextar */
#endif /* MESH_H_ */
