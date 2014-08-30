
#include <FbxImportedMesh.h>
#include <FbxImportedMeshBuffer.h>

void FbxImportedMesh::ParseMaterialConnections(KFbxMesh* pMesh) {
	
	int lPolygonCount = pMesh->GetPolygonCount();
	bool materialsAreSame = true;
	for (int l = 0; l < pMesh->GetElementMaterialCount(); l++)
	{
		KFbxGeometryElementMaterial* materialElement = pMesh->GetElementMaterial(l);
		int lMatId = -1;
		if (materialElement->GetMappingMode() == KFbxGeometryElement::eBY_POLYGON)
		{
			materialsAreSame = false;
			break;
		}
	}

	if (materialsAreSame) {
		if (pMesh->GetElementMaterialCount() > 1)
			Warn("Only the first material will be applied.");

		KFbxSurfaceMaterial* lMaterial = nullptr;
		if (pMesh->GetElementMaterialCount() > 0) {
			KFbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(0);
			if (lMaterialElement->GetMappingMode() == KFbxGeometryElement::eALL_SAME) {
				
				int lMatId = lMaterialElement->GetIndexArray().GetAt(0);
				if (lMatId >= 0) {
					lMaterial = pMesh->GetNode()->GetMaterial(lMatId);
				}
				
			}
		}
		IndexArray polygons;
		polygons.reserve(lPolygonCount);
		for (int i = 0; i < lPolygonCount; i++) {
			polygons.push_back(i);
		}
		CreatePrimitiveGroupFrom(lMaterial, pMesh, polygons);

	} else {
		MaterialPolygonMap materialMap;
		
		for (int i = 0; i < lPolygonCount; i++) {
			for (int l = 0; l < pMesh->GetElementMaterialCount(); l++)	{
				int lMatId = -1;
				KFbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(l);
				lMatId = lMaterialElement->GetIndexArray().GetAt(i);
				
				KFbxSurfaceMaterial* lMaterial = nullptr;
				if (lMatId >= 0)
					lMaterial = pMesh->GetNode()->GetMaterial(lMatId);
				materialMap[lMaterial].push_back(i);
			}
		}

		for (auto &e : materialMap) {
			CreatePrimitiveGroupFrom(e.first, pMesh, e.second);
		}
	}
}

void FbxImportedMesh::CreatePrimitiveGroupFrom(KFbxSurfaceMaterial* material,
	KFbxMesh* pMesh, const IndexArray& polys) {

	// determine vertex channel count
	int colorCount = pMesh->GetElementVertexColorCount();
	int uvCount = pMesh->GetElementUVCount();
	int normalCount = pMesh->GetElementNormalCount();
	int tangentCount = pMesh->GetElementTangentCount();
	int biNormCount = pMesh->GetElementBinormalCount();
	bool useSharedVertexSpace = false;
	if (!numParsedMesh) {
		sharedColorCount = colorCount;
		sharedUVCount = uvCount;
		sharedNormalCount = normalCount;
		sharedTangentCount = tangentCount;
		sharedBiNormCount = biNormCount;
		useSharedVertexSpace = true;
	}
	else if (sharedColorCount == colorCount &&
		sharedUVCount == uvCount 
		/*&& sharedNormalCount == normalCount 
		&& sharedTangentCount == tangentCount 
		&& sharedBiNormCount == biNormCount*/) {
		useSharedVertexSpace = true;
	}
	
	// determine the mesh buffer
	FbxImportedMeshBuffer buffer(uvCount);

	
}
