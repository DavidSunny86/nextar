
#ifndef FBXMESHIMPORTERV10_H_
#define FBXMESHIMPORTERV10_H_

#include <FbxMeshImporter.h>

namespace FbxMeshImporter {

class FbxMeshImporterApp : public nextar::ProjectApplicationContext {
	NEX_LOG_HELPER(FbxMeshImporterApp);
public:

	virtual void Load(InputStreamPtr&, AssetLoader& loader);

	void InitializeLibrary();


	void InitializeSdkObjects(KFbxSdkManager*& pSdkManager, KFbxScene*& pScene);
	void DestroySdkObjects(KFbxSdkManager* pSdkManager);
	bool LoadScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename);

	void ParseContent(KFbxScene* pScene);
	void ParseContent(KFbxNode* pNode);

	void ExportMesh(KFbxNode* pNode);

	FbxMeshImporterApp();
	~FbxMeshImporterApp();

protected:

	bool bExportMesh;
	String kInputPath;
	String kPassword;
};

}

#endif //FBXMESHIMPORTERV10_H_