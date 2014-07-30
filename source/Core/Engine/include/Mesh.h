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
	NEX_LOG_HELPER(Mesh);
public:

	enum Type {
		CLASS_ID = Component::CLASS_MESH, CATAGORY = COMPONENT_CAT(CLASS_ID),
	};

	struct Primitive {
		uint16 renderQueue;
		VisibilityMask visibilityType;
		uint32 sortKey;
		// optimize storage here??
		StreamData stream;
		VisiblePrimitive primitive;

		~Primitive();
	};

	Mesh(const StringID name, const StringID factory, Component* parent = nullptr);
	virtual ~Mesh();

	/* recreate the object with new mesh asset */
	virtual void SetMeshAsset(MeshAssetPtr& asset);
	/** @brief Get node type */
	virtual uint32 GetClassID() const override;
	virtual void Visit(SceneTraversal& traversal) override;

	virtual void SetMaterial(uint32 primitive, MaterialAssetPtr& material);
protected:
	typedef vector<Primitive>::type PrimitiveList;

	PrimitiveList primitives;
	MeshAssetPtr meshAsset;
};

} /* namespace nextar */
#endif /* MESH_H_ */
