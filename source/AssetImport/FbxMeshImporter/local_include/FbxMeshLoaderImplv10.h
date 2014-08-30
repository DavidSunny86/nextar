
#ifndef FBXMESHLOADERIMPLV10_H_
#define FBXMESHLOADERIMPLV10_H_

#include <MeshBuffer.h>
#include <MeshTemplate.h>

namespace FbxMeshImporter {



class FbxMeshLoaderImplv1_0 : public nextar::AssetLoaderImpl {
	NEX_LOG_HELPER(FbxMeshLoaderImplv1_0);

public:
	FbxMeshLoaderImplv1_0(const String& kFilePath, const String& _kPassword);
	virtual ~FbxMeshLoaderImplv1_0();

	virtual void Load(InputStreamPtr&, AssetLoader& loader);

	void InitializeSdkObjects();
	void DestroySdkObjects();
	bool LoadScene();
	void ParseContent(KFbxScene* pScene);
	void ParseContent(KFbxNode* pNode);
	void ParseMaterialConnections(KFbxMesh* pMesh);
	void ExportMesh(KFbxNode* pNode);
	void CreatePrimitiveGroupFrom(
			KFbxSurfaceMaterial* pMtl,
			KFbxMesh* pMesh,
			const IndexArray& polys);

protected:

	typedef map<KFbxSurfaceMaterial*, MeshBuffer*>::type MeshBufferMap;
	typedef map<KFbxSurfaceMaterial*, IndexArray>::type MaterialPolygonMap;

	MeshBuffer* pSharedBuffer;
	MeshBufferMap kMeshMap;
	KFbxSdkManager* pSdkManager;
	KFbxScene* pScene;
	String kPassword;
	String kFilePath;
};

}

#endif //FBXMESHLOADERIMPLV10_H_