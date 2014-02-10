
#include "NexHeaders.h"
#include "BufferManager.h"
#include "Mesh.h"
#include "Material.h"
#include "scene/RegionNode.h"
#include "scene/MeshNode.h"

namespace nextar {

	MeshNode::MeshNode(ComponentManager *creator, const String& name)
			: MovableComponent(creator, name), deformer(0), visibilityMask(0xffffffff),
			  maxDrawDistanceSq(Math::INFINITY) {
	}

	MeshNode::~MeshNode() {
	}

	int MeshNode::GetType() const {
		return TYPE;
	}

	void MeshNode::ResetMesh() {
		mesh.Clear();
		renderMeshes.clear();
	}

	void MeshNode::SetMesh(MeshAssetPtr& _mesh) {
		mesh = _mesh;

		const PrimitiveGroupList& list = mesh->GetPrimitiveGroups();
		renderMeshes.resize(list.size());
		RenderMeshList::iterator it = renderMeshes.begin();

		MeshVertexData* sharedVertexData = mesh->GetSharedVertexData();
		MeshIndexData* sharedIndexData = mesh->GetSharedIndexData();
		MaterialAsset* sharedMaterial = mesh->GetSharedMaterial().GetPtr();
		for(auto &p : list) {
			uint32 sortKey = 0;
			MaterialAsset* material = p.defaultMaterial ? p.defaultMaterial : sharedMaterial;
			ShaderAssetPtr& shader = material->GetShader();
			Renderable& r = (*it);
			r.material = material;
			r.numWorldMatrices = 1;
			r.worldMatrices = &GetMatrixData()->cached;

			MeshVertexData* meshVertexData = p.vertexData ? p.vertexData : sharedVertexData;
			MeshIndexData* meshIndexData = p.indexData ? p.indexData : sharedIndexData;

			VertexData& vd = r.streamData.vertices;

			vd.binding = meshVertexData->binding;
			vd.start = p.startVertex;
			vd.count = p.vertexCount;
			vd.layout = BufferManager::Instance().CreateVertexLayout(				
					meshVertexData->vertexElements,
					meshVertexData->vertexCount
					);

			r.streamData.instanceCount = 1;
			r.streamData.usesOnlyStaticVb = true;

			IndexData& id = r.streamData.indices;
			if (meshIndexData && p.indexCount) {
				id.indices = meshIndexData->ibdata;
				id.start = p.startIndex;
				id.count = p.indexCount;
				r.streamData.useIndices = true;
			} else
				r.streamData.useIndices = false;

			uint8 isTransparent = shader->GetTranslucency();
			r.sortKey = SortKeyHelper::Key(
					material->GetLayerMask(),
					isTransparent,
					0,
					shader->GetShaderMask()
					);
			r.layer = material->GetRenderLayer();
			if (isTransparent)
				SetFlag(TRANSPARENCY_ENABLED);
		}
	}

	void MeshNode::FindVisibles(SceneTraversal& traversal) {
		MovableComponent::FindVisibles(traversal);
		if (traversal.visibilityMask & visibilityMask) {
			uint32 newMask;
			float distance = traversal.camera->GetSqDistance(GetPosition());
			if (distance > maxDrawDistanceSq)
				return;
			uint16 depth = SortKeyHelper::Depth(traversal.recipDistanceInView * distance);
			BoundingVolume& bv = *GetBounds();
			if (traversal.camera->IsVisible(bv, newMask,
					lastFrustumPlane)) {
				/* Register to queues */
				// we can copy the calculated matrix to the back buffer here
				for (auto &r : renderMeshes) {
					r.sortKey = SortKeyHelper::KeyUpdate(r.sortKey, depth);
					traversal.visibilitySet->AddVisiblePrimitve(r.layer, &r, r.sortKey);
				}
				if (traversal.visibleBoundsInfo)
					traversal.visibleBoundsInfo->MergeBounds(*GetBounds());
			}
		}
	}
}
