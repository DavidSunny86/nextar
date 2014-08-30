
#ifndef IMPORTEDMESH_H_
#define IMPORTEDMESH_H_

#include <FbxMeshImporter.h>
#include <ImportedMesh.h>

class FbxImportedMesh : public AllocGeneral {
	NEX_LOG_HELPER(FbxImportedMesh);
public:

	void ParseMaterialConnections(KFbxMesh* pMesh);
	void ParsePolygons(KFbxMesh* pMesh);

	void CreatePrimitiveGroupFrom(KFbxSurfaceMaterial* material, KFbxMesh* pMesh, const IndexArray& polys); 
protected:

	int numParsedMesh;
	int sharedColorCount;
	int sharedUVCount;
	int sharedNormalCount;
	int sharedTangentCount;
	int sharedBiNormCount;
		
};

#endif //IMPORTEDMESH_H_