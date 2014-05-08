/*
 * RenderableMesh.cpp
 *
 *  Created on: 25-Jan-2014
 *      Author: obhi
 */
#include <NexEngine.h>
#include <MaterialAsset.h>
#include <ShaderAsset.h>
#include <MeshAsset.h>
#include <Mesh.h>
#include <SceneTraversal.h>
#include <Camera.h>

namespace nextar {

	Mesh::Primitive::~Primitive() {
	}

	/************************************************************************/
	/* Mesh                                                                 */
	/************************************************************************/
	Mesh::Mesh(const StringID name, Component* parent) :
			Renderable(name, parent), defaultRenderQueue(0) {
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
				prim.primitive.SetWorldMatrices(&GetWorldMatrix(), 1);
				prim.primitive.SetBoundsInfo(&GetBoundsInfo());
				prim.layer = 0;
				prim.sortKey = SortKeyHelper::KeyOpaque(prim.layer, 0, 0);
				primitives.push_back(prim);
				SetMaterial((uint32)primitives.size()-1, mtl);
			}
		}
	}

	void Mesh::SetMaterial(uint32 prim, MaterialAssetPtr& m) {
		Primitive& p = primitives[prim];
		ShaderAssetPtr& s = m->GetShader();
		p.primitive.SetMaterial(m);
		p.sortKey = SortKeyHelper::KeyOpaque(p.layer, 0, s->GetShaderMask());
	}

	uint32 Mesh::GetClassID() const {
		return CLASS_ID;
	}
} /* namespace nextar */
