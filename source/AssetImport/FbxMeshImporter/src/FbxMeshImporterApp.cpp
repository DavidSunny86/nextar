
#include <FbxMeshImporterApp.h>

namespace FbxMeshImporter {

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pSdkManager->GetIOSettings()))
#endif

FbxMeshImporterApp::FbxMeshImporterApp() : ProjectApplicationContext("FbxMeshImporterApp") {

}

FbxMeshImporterApp::~FbxMeshImporterApp() {

}

void FbxMeshImporterApp::InitializeSdkObjects(KFbxSdkManager*& pSdkManager, KFbxScene*& pScene) {
	// The first thing to do is to create the FBX SDK manager which is the 
	// object allocator for almost all the classes in the SDK.
	pSdkManager = KFbxSdkManager::Create();

	if (!pSdkManager) {
		Warn("Unable to create the FBX SDK manager\n");
		return;
	}

	// create an IOSettings object
	KFbxIOSettings * ios = KFbxIOSettings::Create(pSdkManager, IOSROOT);
	pSdkManager->SetIOSettings(ios);

	// Load plugins from the executable directory
	KString lPath = KFbxGetApplicationDirectory();
#if defined(KARCH_ENV_WIN)
	KString lExtension = "dll";
#elif defined(KARCH_ENV_MACOSX)
	KString lExtension = "dylib";
#elif defined(KARCH_ENV_LINUX)
	KString lExtension = "so";
#endif
	pSdkManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

	// Create the entity that will hold the scene.
	pScene = KFbxScene::Create(pSdkManager, "");
}

void FbxMeshImporterApp::DestroySdkObjects(KFbxSdkManager* pSdkManager) {
	// Delete the FBX SDK manager. All the objects that have been allocated 
	// using the FBX SDK manager and that haven't been explicitly destroyed 
	// are automatically destroyed at the same time.
	if (pSdkManager) pSdkManager->Destroy();
	pSdkManager = NULL;
}

bool FbxMeshImporterApp::LoadScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename) {
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	KFbxSdkManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	KFbxImporter* lImporter = KFbxImporter::Create(pSdkManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pSdkManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus) {
		Warn("Call to KFbxImporter::Initialize() failed.\n");
		Warn(String("Error returned: ") + lImporter->GetLastErrorString());

		if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
			lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE) {
			VersionID fbxVersion = NEX_VERSION(lSDKMajor, lSDKMinor, lSDKRevision);
			VersionID fileVersion = NEX_VERSION(lFileMajor, lFileMinor, lFileRevision);
			Trace(String("FBX version number for this FBX SDK is: ") + Convert::ToVersionString(fbxVersion));
			Trace(String("FBX version number for the input file: ") + Convert::ToVersionString(fileVersion));
		}

		return false;
	}
	
	if (lImporter->IsFBX())	{
		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.
		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);

	if (lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)	{
	
		KString lString(kPassword.c_str());

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
		{
			Warn("Password is wrong, import aborted.");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

void FbxMeshImporterApp::ParseContent(KFbxScene* pScene)
{
	int i;
	KFbxNode* lNode = pScene->GetRootNode();

	if (lNode)
	{
		for (i = 0; i < lNode->GetChildCount(); i++)
		{
			ParseContent(lNode->GetChild(i));
		}
	}
}

void FbxMeshImporterApp::ParseContent(KFbxNode* pNode)
{
	KFbxNodeAttribute::EAttributeType lAttributeType;
	int i;
	KFbxMesh *pMesh;
			
	if (pNode->GetNodeAttribute() == NULL) {
		printf("NULL Node Attribute\n\n");
	}
	else {
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

		switch (lAttributeType)	{
		case KFbxNodeAttribute::eMARKER:
			break;

		case KFbxNodeAttribute::eSKELETON:
			break;

		case KFbxNodeAttribute::eMESH:
			if (bExportMesh)
				ExportMesh(pNode);
			break;

		case KFbxNodeAttribute::eNURB:
			break;

		case KFbxNodeAttribute::ePATCH:
			break;

		case KFbxNodeAttribute::eCAMERA:
			break;

		case KFbxNodeAttribute::eLIGHT:
			break;

		case KFbxNodeAttribute::eLODGROUP:
			break;
		}
	}

	for (i = 0; i < pNode->GetChildCount(); i++) {
		ParseContent(pNode->GetChild(i));
	}
}

void FbxMeshImporterApp::ExportMesh(KFbxNode* pNode) {

}

void FbxMeshImporterApp::InitializeLibrary() {
	KFbxSdkManager* lSdkManager = nullptr;
	KFbxScene* lScene = nullptr;

	InitializeSdkObjects(lSdkManager, lScene);
	LoadScene(lSdkManager, lScene, kInputPath.c_str());

	ParseContent(lScene);
}


void DisplayPolygons(KFbxMesh* pMesh)
{
	int i, j, lPolygonCount = pMesh->GetPolygonCount();
	KFbxVector4* lControlPoints = pMesh->GetControlPoints();
	char header[100];

	DisplayString("    Polygons");

	int vertexId = 0;
	for (i = 0; i < lPolygonCount; i++)
	{
		DisplayInt("        Polygon ", i);
		int l;

		int lPolygonSize = pMesh->GetPolygonSize(i);

		for (j = 0; j < lPolygonSize; j++)
		{
			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);

			Display3DVector("            Coordinates: ", lControlPoints[lControlPointIndex]);

			for (l = 0; l < pMesh->GetElementVertexColorCount(); l++)
			{
				KFbxGeometryElementVertexColor* leVtxc = pMesh->GetElementVertexColor(l);
				sprintf(header, "            Color vertex: ");

				switch (leVtxc->GetMappingMode())
				{
				case KFbxGeometryElement::eBY_CONTROL_POINT:
					switch (leVtxc->GetReferenceMode())
					{
					case KFbxGeometryElement::eDIRECT:
						DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));
						break;
					case KFbxGeometryElement::eINDEX_TO_DIRECT:
					{
																  int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
																  DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
					}
						break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case KFbxGeometryElement::eBY_POLYGON_VERTEX:
				{
																switch (leVtxc->GetReferenceMode())
																{
																case KFbxGeometryElement::eDIRECT:
																	DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
																	break;
																case KFbxGeometryElement::eINDEX_TO_DIRECT:
																{
																											  int id = leVtxc->GetIndexArray().GetAt(vertexId);
																											  DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
																}
																	break;
																default:
																	break; // other reference modes not shown here!
																}
				}
					break;

				case KFbxGeometryElement::eBY_POLYGON: // doesn't make much sense for UVs
				case KFbxGeometryElement::eALL_SAME:   // doesn't make much sense for UVs
				case KFbxGeometryElement::eNONE:       // doesn't make much sense for UVs
					break;
				}
			}
			for (l = 0; l < pMesh->GetElementUVCount(); ++l)
			{
				KFbxGeometryElementUV* leUV = pMesh->GetElementUV(l);
				sprintf(header, "            Texture UV: ");

				switch (leUV->GetMappingMode())
				{
				case KFbxGeometryElement::eBY_CONTROL_POINT:
					switch (leUV->GetReferenceMode())
					{
					case KFbxGeometryElement::eDIRECT:
						Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));
						break;
					case KFbxGeometryElement::eINDEX_TO_DIRECT:
					{
																  int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
																  Display2DVector(header, leUV->GetDirectArray().GetAt(id));
					}
						break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case KFbxGeometryElement::eBY_POLYGON_VERTEX:
				{
																int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
																switch (leUV->GetReferenceMode())
																{
																case KFbxGeometryElement::eDIRECT:
																case KFbxGeometryElement::eINDEX_TO_DIRECT:
																{
																											  Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));
																}
																	break;
																default:
																	break; // other reference modes not shown here!
																}
				}
					break;

				case KFbxGeometryElement::eBY_POLYGON: // doesn't make much sense for UVs
				case KFbxGeometryElement::eALL_SAME:   // doesn't make much sense for UVs
				case KFbxGeometryElement::eNONE:       // doesn't make much sense for UVs
					break;
				}
			}
			for (l = 0; l < pMesh->GetElementNormalCount(); ++l)
			{
				KFbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(l);
				sprintf(header, "            Normal: ");

				if (leNormal->GetMappingMode() == KFbxGeometryElement::eBY_POLYGON_VERTEX)
				{
					switch (leNormal->GetReferenceMode())
					{
					case KFbxGeometryElement::eDIRECT:
						Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
						break;
					case KFbxGeometryElement::eINDEX_TO_DIRECT:
					{
																  int id = leNormal->GetIndexArray().GetAt(vertexId);
																  Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
					}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for (l = 0; l < pMesh->GetElementTangentCount(); ++l)
			{
				KFbxGeometryElementTangent* leTangent = pMesh->GetElementTangent(l);
				sprintf(header, "            Tangent: ");

				if (leTangent->GetMappingMode() == KFbxGeometryElement::eBY_POLYGON_VERTEX)
				{
					switch (leTangent->GetReferenceMode())
					{
					case KFbxGeometryElement::eDIRECT:
						Display3DVector(header, leTangent->GetDirectArray().GetAt(vertexId));
						break;
					case KFbxGeometryElement::eINDEX_TO_DIRECT:
					{
																  int id = leTangent->GetIndexArray().GetAt(vertexId);
																  Display3DVector(header, leTangent->GetDirectArray().GetAt(id));
					}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for (l = 0; l < pMesh->GetElementBinormalCount(); ++l)
			{

				KFbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal(l);

				sprintf(header, "            Binormal: ");
				if (leBinormal->GetMappingMode() == KFbxGeometryElement::eBY_POLYGON_VERTEX)
				{
					switch (leBinormal->GetReferenceMode())
					{
					case KFbxGeometryElement::eDIRECT:
						Display3DVector(header, leBinormal->GetDirectArray().GetAt(vertexId));
						break;
					case KFbxGeometryElement::eINDEX_TO_DIRECT:
					{
																  int id = leBinormal->GetIndexArray().GetAt(vertexId);
																  Display3DVector(header, leBinormal->GetDirectArray().GetAt(id));
					}
						break;
					default:
						break; // other reference modes not shown here!
					}
				}
			}
			vertexId++;
		} // for polygonSize
	} // for polygonCount


	//check visibility for the edges of the mesh
	for (int l = 0; l < pMesh->GetElementVisibilityCount(); ++l)
	{
		KFbxGeometryElementVisibility* leVisibility = pMesh->GetElementVisibility(l);
		sprintf(header, "    Edge Visibility : ");
		DisplayString(header);
		switch (leVisibility->GetMappingMode())
		{
			//should be eBY_EDGE
		case KFbxGeometryElement::eBY_EDGE:
			//should be eDIRECT
			for (int j = 0; j != pMesh->GetMeshEdgeCount(); ++j)
			{
				DisplayInt("        Edge ", j);
				DisplayBool("              Edge visibility: ", leVisibility->GetDirectArray().GetAt(j));
			}

			break;
		}
	}
	DisplayString("");
}

}

