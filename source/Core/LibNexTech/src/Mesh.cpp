/*
 * RenderableMesh.cpp
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "Mesh.h"

namespace nextar {

	Mesh::Mesh(ComponentManager* creator, const String& name, Component* parent) :
			Renderable(creator, name, parent), defaultRenderQueue(0) {
	}

	Mesh::~Mesh() {
	}

	void Mesh::Visit(SceneTraversal& traversal) {
		/* cheap integer trick not possible because we use shader key too */
		float depth = traversal.camera->GetNormalizedDepth(GetPosition());
		for (auto &p : primitives) {
			traversal.visibilitySet->AddVisiblePrimitve(p.renderQueue, &p.primitive,
					SortKeyHelper::KeyOpaque(p.sortKey, depth));
		}
	}

	void Mesh::SetMeshAsset(MeshAssetPtr& mesh) {
		meshAsset = mesh;
		primitives.clear();
		if (meshAsset) {

			PrimitiveGroupList& groups = mesh->GetPrimitiveGroups();
			primitives.reserve(groups.size());
			for(auto &pg : groups) {
				MaterialAssetPtr& mtl = pg.defaultMaterial;
				Primitive prim;
				prim.flags = 0;
				prim.renderQueue = defaultRenderQueue;
				prim.primitive.SetWorldMatrices(GetWorldMatrix(), 1);
				prim.primitive.SetBoundsInfo(&GetBoundsInfo());
				prim.layer = 0;
				prim.sortKey = SortKeyHelper::KeyOpaque(prim.layer, 0, 0);
				primitives.push_back(prim);
				SetMaterial(primitives.size()-1, mtl);
			}
		}
	}

	void Mesh::SetMaterial(uint32 prim, MaterialAssetPtr& m) {
		Primitive& p = primitives[prim];
		ShaderAssetPtr& s = m->GetShader();
		p.primitive.SetMaterial(m);
		p.sortKey = SortKeyHelper::KeyOpaque(p.layer, 0, s->GetShaderMask());
	}
} /* namespace nextar */
