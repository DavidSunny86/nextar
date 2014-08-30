
#include <FbxMeshImporter.h>
#include <FbxMeshLoaderImplv10.h>

namespace FbxMeshImporter {

FbxMeshLoaderImplv1_0::FbxMeshLoaderImplv1_0(const String& _kFilePath, const String& _kPassword) :
kFilePath(_kFilePath)
, kPassword(_kPassword)
, pSdkManager(NULL)
, pScene(NULL) {
}

FbxMeshLoaderImplv1_0::~FbxMeshLoaderImplv1_0() {

}

void FbxMeshLoaderImplv1_0::InitializeSdkObjects() {
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

void FbxMeshLoaderImplv1_0::DestroySdkObjects() {
	// Delete the FBX SDK manager. All the objects that have been allocated 
	// using the FBX SDK manager and that haven't been explicitly destroyed 
	// are automatically destroyed at the same time.
	if (pSdkManager) pSdkManager->Destroy();
	pSdkManager = NULL;
}

bool FbxMeshLoaderImplv1_0::LoadScene() {
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
	const bool lImportStatus = lImporter->Initialize(kFilePath.c_str(), -1, pSdkManager->GetIOSettings());
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


void FbxMeshLoaderImplv1_0::ParseContent(KFbxScene* pScene)
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

void FbxMeshLoaderImplv1_0::ParseContent(KFbxNode* pNode)
{
	KFbxNodeAttribute::EAttributeType lAttributeType;
	int i;

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

void FbxMeshLoaderImplv1_0::ExportMesh(KFbxNode* pNode) {
	KFbxMesh* pMesh = (KFbxMesh*)pNode->GetNodeAttribute();
	if (!pMesh->IsTriangleMesh()) {
		Warn("Non triangular mesh encountered: " + String(pMesh->GetName()));
		return;
	}
}

void FbxMeshLoaderImplv1_0::ParseMaterialConnections(KFbxMesh* pMesh) {

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

	}
	else {
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

template <typename SrcType, typename DstType>
DstType Convert(const SrcType& t) {
	return (DstType)t;
}

template <>
Vector3 Convert<KFbxVector4, Vector3>(const KFbxVector4& v) {
	return Vector3((float)v[0], (float)v[1], (float)v[2]);
}

template <>
Vector2 Convert<KFbxVector2, Vector2>(const KFbxVector2& v) {
	return Vector2((float)v[0], (float)v[1]);
}

template <>
uint32 Convert<KFbxColor, uint32>(const KFbxColor& v) {
	Color32 c((float)v.mAlpha, (float)v.mRed, (float)v.mGreen, (float)v.mBlue);
	return c.ToRgba();
}

template <typename SrcType, typename DstType, typename FbxLayerType>
void CopyVertexChannel(KFbxMesh* pMesh, 
	FbxLayerType* layer,
	VertexChannel* channel, 
	const IndexArray& polys) {
	for (auto p : polys) {
		NEX_ASSERT(pMesh->GetPolygonSize(p) == 3);
			
		for (uint32 j = 0; j < 3; ++j) {
			uint32 index = pMesh->GetPolygonVertex(p, j);
			DstType value;

			switch (layer->GetMappingMode()) {
			case KFbxGeometryElement::eBY_CONTROL_POINT:
				switch (layer->GetReferenceMode()) {
				case KFbxGeometryElement::eDIRECT:
				{
					
					value = Convert<SrcType, DstType>(layer->GetDirectArray().GetAt(index));
					channel->PushVertex(&value);
				}
					break;
				case KFbxGeometryElement::eINDEX_TO_DIRECT:
				{
					int32 id = layer->GetIndexArray().GetAt(index);
					value = Convert<SrcType, DstType>(layer->GetDirectArray().GetAt(id));
					channel->PushVertex(&value);
				}
					break;
				default:
					break; // other reference modes not shown here!
				}
				break;

			case KFbxGeometryElement::eBY_POLYGON_VERTEX:
			
				switch (leVtxc->GetReferenceMode()) {
				case KFbxGeometryElement::eDIRECT: 
				{
					uint32 vertexId = p * 3 + j;
					value = Convert<SrcType, DstType>(layer->GetDirectArray().GetAt(vertexId));
					break;
				}
				case KFbxGeometryElement::eINDEX_TO_DIRECT:	
				{
					uint32 vertexId = p * 3 + j;
					int id = layer->GetIndexArray().GetAt(vertexId);
					value = Convert<SrcType, DstType>(layer->GetDirectArray().GetAt(vertexId));
				}
					break;
				default:
					break; // other reference modes not shown here!
				}
			
				break;

			case KFbxGeometryElement::eBY_POLYGON: // doesn't make much sense for UVs
			case KFbxGeometryElement::eALL_SAME:   // doesn't make much sense for UVs
			case KFbxGeometryElement::eNONE:       // doesn't make much sense for UVs
				break;
			}

			channel->PushVertex(&value);
		}
	}
}

void FbxMeshLoaderImplv1_0::CreatePrimitiveGroupFrom(
	KFbxSurfaceMaterial* pMtl,
	KFbxMesh* pMesh, 
	const IndexArray& polys) {

	// determine vertex channel count
	int uvChannelCount = pMesh->GetElementUVCount();
	int normalCount = pMesh->GetElementNormalCount();
	int biNormalCount = pMesh->GetElementBinormalCount();
	int tangentCount = pMesh->GetElementTangentCount();
	int colorCount = pMesh->GetElementVertexColorCount();

	bool useSharedVertexSpace = false;
	if (!pSharedBuffer) {
		useSharedVertexSpace = true;
	}
	else if (pSharedBuffer->GetChannelCount(COMP_NORMAL) == (uint32)normalCount &&
		pSharedBuffer->GetChannelCount(COMP_BINORMAL) == (uint32)biNormalCount &&
		pSharedBuffer->GetChannelCount(COMP_TANGENT) == (uint32)tangentCount &&
		pSharedBuffer->GetChannelCount(COMP_COLOR) == (uint32)colorCount &&
		pSharedBuffer->GetChannelCount(COMP_TEXTURE_COORDINATE) == (uint32)uvChannelCount) {
		useSharedVertexSpace = true;
	}
	
	MeshBuffer* pBuffer = nullptr;
	if (useSharedVertexSpace) {
		pBuffer = pSharedBuffer;
	} else {
		auto it = kMeshMap.find(pMtl);
		if (it != kMeshMap.end()) {
			if ((*it).second->GetChannelCount(COMP_NORMAL) == (uint32)normalCount  &&
				(*it).second->GetChannelCount(COMP_BINORMAL) == (uint32)biNormalCount &&
				(*it).second->GetChannelCount(COMP_TANGENT) == (uint32)tangentCount &&
				(*it).second->GetChannelCount(COMP_COLOR) == (uint32)colorCount &&
				(*it).second->GetChannelCount(COMP_TEXTURE_COORDINATE) 	== (uint32)uvChannelCount) {
				pBuffer = (*it).second;
			}
		}
	}
	
	// determine the mesh buffer
	if (!pBuffer) {
		pBuffer = NEX_NEW(MeshBuffer(PrimitiveType::PT_TRI_LIST));
		kMeshMap.emplace(pMtl, pBuffer);
		pBuffer->AddVertexChannel(COMP_POSITION, 0, COMP_TYPE_FLOAT3);
		if (normalCount > 0)
			pBuffer->AddVertexChannel(COMP_NORMAL, 0, COMP_TYPE_FLOAT3);
		if (tangentCount > 0)
			pBuffer->AddVertexChannel(COMP_TANGENT, 0, COMP_TYPE_FLOAT3);
		if (biNormalCount > 0)
			pBuffer->AddVertexChannel(COMP_BINORMAL, 0, COMP_TYPE_FLOAT3);

		if (colorCount > 0) {
			for (int32 i = 0; i < colorCount; ++i)
				pBuffer->AddVertexChannel(COMP_COLOR, (uint32)i, COMP_TYPE_COLOR);
		}

		if (uvChannelCount > 0) {
			for (int32 i = 0; i < uvChannelCount; ++i) {
				pBuffer->AddVertexChannel(COMP_TEXTURE_COORDINATE, (uint32)i, COMP_TYPE_FLOAT2);
			}
		}

		if (useSharedVertexSpace)
			pSharedBuffer = pBuffer;
	}
	
	pBuffer->ReserveVertexSpace(polys.size() * 3);
	pBuffer->ReserveIndexSpace(polys.size() * 3);

	uint32 baseVertexIdx = pBuffer->GetVertexCount();
	KFbxVector4* lControlPoints = pMesh->GetControlPoints();
	VertexChannel* pPosition = pBuffer->GetVertexChannel(
		pBuffer->AddVertexChannel(COMP_POSITION, 0, COMP_TYPE_FLOAT3)
		);

	for (auto p : polys) {
		uint32 lPolygonSize = pMesh->GetPolygonSize(p);
		NEX_ASSERT(lPolygonSize == 3);
		for (uint32 j = 0; j < 3; j++) {
			uint32 index = pMesh->GetPolygonVertex(p, j);
			auto &kV = lControlPoints[index];
			float fVertices[3] = {
				(float)kV.mData[0],
				(float)kV.mData[1],
				(float)kV.mData[2]
			};
			pPosition->PushVertex(fVertices);
			pBuffer->PushIndex(baseVertexIdx++);
		}
		
	}

	if (normalCount > 0) {
		KFbxGeometryElementNormal* pFbxNormalLayer = pMesh->GetElementNormal(0);
		VertexChannel* pNormalChannel = pBuffer->GetVertexChannel(COMP_NORMAL, 0);
		CopyVertexChannel<KFbxVector4, Vector3>(pMesh, pFbxNormalLayer, pNormalChannel, polys);
	}

	if (tangentCount > 0) {
		KFbxGeometryElementTangent* pFbxTangentLayer = pMesh->GetElementTangent(0);
		VertexChannel* pTangentChannel = pBuffer->GetVertexChannel(COMP_TANGENT, 0);
		CopyVertexChannel<KFbxVector4, Vector3>(pMesh, pFbxTangentLayer, pTangentChannel, polys);
	}

	if (biNormalCount > 0) {
		KFbxGeometryElementBinormal* pFbxBinormalLayer = pMesh->GetElementBinormal(0);
		VertexChannel* pBinormalChannel = pBuffer->GetVertexChannel(COMP_TANGENT, 0);
		CopyVertexChannel<KFbxVector4, Vector3>(pMesh, pFbxBinormalLayer, pBinormalChannel, polys);
	}

	
	for (int32 uv = 0; uv < uvChannelCount; ++uv) {
		KFbxGeometryElementUV* pFbxUVChannel = pMesh->GetElementUV(uv);
		VertexChannel* pUVChannel = pBuffer->GetVertexChannel(COMP_TEXTURE_COORDINATE, (uint32)uv);
		CopyVertexChannel<KFbxVector2, Vector2>(pMesh, pFbxUVChannel, pUVChannel, polys);
	}

	for (int32 c = 0; c < colorCount; ++c) {
		KFbxGeometryElementVertexColor* pFbxColorChannel = pMesh->GetElementVertexColor(c);
		VertexChannel* pColorChannel = pBuffer->GetVertexChannel(COMP_COLOR, (uint32)c);
		CopyVertexChannel<KFbxColor, uint32>(pMesh, pFbxColorChannel, pColorChannel, polys);
	}
}

void FbxMeshLoaderImplv1_0::Load(InputStreamPtr&, AssetLoader& loader) {

}

}