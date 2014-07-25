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
		Renderable(name, parent) {
}

Mesh::~Mesh() {
}

void Mesh::Visit(SceneTraversal& traversal) {
	/* cheap integer trick not possible because we use shader key too */
	float depth = traversal.camera->GetNormalizedDepth(GetTranslation());
	for (auto &p : primitives) {
		if (p.visibilityType & traversal.visibilityMask) {
			traversal.visibilitySet->AddVisiblePrimitve(p.renderQueue,
					SortKeyHelper::KeyOpaque(p.sortKey, depth), &p.primitive);
		}
	}
}

void Mesh::SetMeshAsset(MeshAssetPtr& mesh) {
	meshAsset = mesh;
	primitives.clear();
	if (meshAsset) {

		PrimitiveGroupList& groups = mesh->GetPrimitiveGroups();
		primitives.reserve(groups.size());
		for (auto &pg : groups) {
			MaterialAssetPtr& mtl = pg.defaultMaterial;
			Primitive prim;
			prim.primitive.SetWorldMatrices(&GetWorldMatrix(), 1);
			prim.primitive.SetBoundsInfo(&GetBoundsInfo());
			prim.primitive.SetStreamData(&prim.stream);
			prim.stream.type = pg.primitiveType;
			prim.stream.instanceCount = 1;
			prim.stream.indices.start = pg.startIndex;
			prim.stream.indices.count = pg.indexCount;
			prim.stream.indices.indices = Bind(&pg.indexData->ibdata);
			prim.stream.vertices.start = pg.startVertex;
			prim.stream.vertices.count = pg.vertexCount;
			prim.stream.vertices.layout = pg.vertexData->layout;
			prim.stream.vertices.binding = pg.vertexData->binding;
			prim.visibilityType = VisibilityMask::VISIBILITY_SHADOW_CASTER|
					VisibilityMask::VISIBILITY_SHADOW_RECEIVER;
			//prim.sortKey = SortKeyHelper::KeyOpaque(prim.layer, 0, 0);
			primitives.push_back(prim);
			SetMaterial((uint32) primitives.size() - 1, mtl);
		}
	}
}

void Mesh::SetMaterial(uint32 prim, MaterialAssetPtr& m) {
	if (prim >= primitives.size()) {
		Warn("Invalid primitive index to set material.");
		return;
	}
	Primitive& p = primitives[prim];
	ShaderAssetPtr& s = m->GetShader();
	p.primitive.SetMaterial(m);
	p.sortKey = SortKeyHelper::KeyOpaque(m->GetLayerMask(), 0, s->GetShaderMask());
	VisibilityMask oldMask = p.visibilityType;
	p.visibilityType = s->GetVisibilityMask();
	if (VisibilityMask::VISIBILITY_SHADOW_CASTER & oldMask)
		p.visibilityType |= VisibilityMask::VISIBILITY_SHADOW_CASTER;
	if (VisibilityMask::VISIBILITY_SHADOW_RECEIVER & oldMask)
		p.visibilityType |= VisibilityMask::VISIBILITY_SHADOW_RECEIVER;
	p.renderQueue = s->GetRenderQueue();
}

uint32 Mesh::GetClassID() const {
	return CLASS_ID;
}
} /* namespace nextar */
