
#ifndef FBXMESHLOADERIMPLV10_H_
#define FBXMESHLOADERIMPLV10_H_

#include <MeshBuffer.h>
#include <MeshTemplate.h>
#include <FbxInputStreamBridge.h>

namespace FbxMeshImporter {


class FbxMeshLoaderImplv1_0 : public AllocGeneral {
	NEX_LOG_HELPER(FbxMeshLoaderImplv1_0);

public:
	struct MeshElement {
		uint32 mStartVertex;
		uint32 mVertexCount;
		uint32 mStartIndex;
		uint32 mIndexCount;
		FbxSurfaceMaterial* mMaterial;
		MeshBuffer* mMesh;
	};

	FbxMeshLoaderImplv1_0(FbxManager* pManager, InputStreamPtr& pInput, const String& kPassword);
	virtual ~FbxMeshLoaderImplv1_0();

	bool LoadScene();
	void ParseContent();
	void ParseContent(FbxNode* pNode);
	void ParseMaterialConnections(FbxMesh* pMesh);
	void ExportMesh(FbxNode* pNode);
	void CreatePrimitiveGroupFrom(
			FbxSurfaceMaterial* pMtl,
			FbxMesh* pMesh,
			const IndexArray& polys);

	void BuildMesh(MeshTemplate::MeshBuilder* pMesh);
	MaterialTemplatePtr CreateMaterial(FbxSurfaceMaterial*);

protected:

	typedef vector<MeshElement>::type MeshElementList;
	typedef map<FbxSurfaceMaterial*, IndexArray>::type MaterialPolygonMap;

	FbxInputStreamBridge mStream;
	MeshBuffer* mSharedBuffer;
	MeshElementList mElements;
	FbxManager* mManager;
	FbxScene* mScene;
	String mPassword;
};

}

#endif //FBXMESHLOADERIMPLV10_H_
