
#include <FbxMeshImporter.h>
#include <FbxMeshLoaderImplv10.h>
#include <FbxMaterialLoaderImplv10.h>

namespace FbxMeshImporter {
#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(mManager->GetIOSettings()))
#endif

FbxMeshLoaderImplv1_0::FbxMeshLoaderImplv1_0(FbxManager* pManager,
		InputStreamPtr& pInput, const String& kPassword) :
		mManager(pManager), mPassword(kPassword), mScene(nullptr), mSharedBuffer(
				nullptr), mStream(pInput, GetFbxReaderID(pManager)) {
	mScene = FbxScene::Create(mManager, "");
}

FbxMeshLoaderImplv1_0::~FbxMeshLoaderImplv1_0() {
	mScene->Destroy(true);
}

int FbxMeshLoaderImplv1_0::GetFbxReaderID(FbxManager* pManager) {
	return pManager->GetIOPluginRegistry()->FindReaderIDByExtension("fbx");
}

bool FbxMeshLoaderImplv1_0::LoadScene() {
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	//int lFileFormat = -1;
	bool lStatus;

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(mManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(&mStream, nullptr, -1,
			mManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus) {
		Warn("Call to FbxImporter::Initialize() failed.\n");
		Warn(
				String("Error returned: ")
						+ lImporter->GetStatus().GetErrorString());

		if (lImporter->GetStatus().GetCode()
				== FbxStatus::eInvalidFileVersion) {
			VersionID fbxVersion = NEX_MAKE_VERSION(lSDKMajor, lSDKMinor,
					lSDKRevision);
			VersionID fileVersion = NEX_MAKE_VERSION(lFileMajor, lFileMinor,
					lFileRevision);
			Trace(
					String("FBX version number for this FBX SDK is: ")
							+ Convert::ToVersionString(fbxVersion));
			Trace(
					String("FBX version number for the input file: ")
							+ Convert::ToVersionString(fileVersion));
		}

		return false;
	}

	if (lImporter->IsFBX()) {
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
	lStatus = lImporter->Import(mScene);

	if (lStatus == false
			&& lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError) {

		FbxString lString(mPassword.c_str());

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(mScene);

		if (lStatus == false
				&& lImporter->GetStatus().GetCode()
						== FbxStatus::ePasswordError) {
			Warn("Password is wrong, import aborted.");
		}
	}
	if (lStatus) {
		FbxGeometryConverter converter(mManager);
		if(!converter.Triangulate(mScene, true)) {
			Warn("Not all meshes were converted!");
		}
	}
	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

void FbxMeshLoaderImplv1_0::ParseContent() {
	int i;
	FbxNode* lNode = mScene->GetRootNode();

	if (lNode) {
		for (i = 0; i < lNode->GetChildCount(); i++) {
			ParseContent(lNode->GetChild(i));
		}
	}
}

void FbxMeshLoaderImplv1_0::ParseContent(FbxNode* pNode) {
	FbxNodeAttribute::EType lAttributeType;
	int i;

	if (pNode->GetNodeAttribute() == NULL) {
		printf("NULL Node Attribute\n\n");
	} else {
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

		switch (lAttributeType) {
		case FbxNodeAttribute::eMarker:
			break;

		case FbxNodeAttribute::eSkeleton:
			break;

		case FbxNodeAttribute::eMesh:
			ExportMesh(pNode);
			break;

		case FbxNodeAttribute::eNurbs:
			break;

		case FbxNodeAttribute::ePatch:
			break;

		case FbxNodeAttribute::eCamera:
			break;

		case FbxNodeAttribute::eLight:
			break;

		case FbxNodeAttribute::eLODGroup:
			break;
		}
	}

	for (i = 0; i < pNode->GetChildCount(); i++) {
		ParseContent(pNode->GetChild(i));
	}
}

void FbxMeshLoaderImplv1_0::ExportMesh(FbxNode* pNode) {
	FbxMesh* pMesh = (FbxMesh*) pNode->GetNodeAttribute();
	if (!pMesh->IsTriangleMesh()) {
		Warn("Non triangular mesh encountered: " + String(pMesh->GetName()));
		return;
	}
	ParseMaterialConnections(pMesh);
}

void FbxMeshLoaderImplv1_0::ParseMaterialConnections(FbxMesh* pMesh) {

	int lPolygonCount = pMesh->GetPolygonCount();
	bool materialsAreSame = true;
	for (int l = 0; l < pMesh->GetElementMaterialCount(); l++) {
		FbxGeometryElementMaterial* materialElement = pMesh->GetElementMaterial(
				l);
		int lMatId = -1;
		if (materialElement->GetMappingMode()
				== FbxGeometryElement::eByPolygon) {
			materialsAreSame = false;
			break;
		}
	}

	if (materialsAreSame) {
		if (pMesh->GetElementMaterialCount() > 1)
			Warn("Only the first material will be applied.");

		FbxSurfaceMaterial* lMaterial = nullptr;
		if (pMesh->GetElementMaterialCount() > 0) {
			FbxGeometryElementMaterial* lMaterialElement =
					pMesh->GetElementMaterial(0);
			if (lMaterialElement->GetMappingMode()
					== FbxGeometryElement::eAllSame) {

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
			for (int l = 0; l < pMesh->GetElementMaterialCount(); l++) {
				int lMatId = -1;
				FbxGeometryElementMaterial* lMaterialElement =
						pMesh->GetElementMaterial(l);
				lMatId = lMaterialElement->GetIndexArray().GetAt(i);

				FbxSurfaceMaterial* lMaterial = nullptr;
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

template<typename SrcType, typename DstType>
DstType Convert(const SrcType& t) {
	return (DstType) t;
}

template<>
Vector3 Convert<FbxVector4, Vector3>(const FbxVector4& v) {
	return Vector3((float) v[0], (float) v[1], (float) v[2]);
}

template<>
Vector2 Convert<FbxVector2, Vector2>(const FbxVector2& v) {
	return Vector2((float) v[0], (float) v[1]);
}

template<>
uint32 Convert<FbxColor, uint32>(const FbxColor& v) {
	Color32 c((float) v.mAlpha, (float) v.mRed, (float) v.mGreen,
			(float) v.mBlue);
	return c.ToRgba();
}

template<typename SrcType, typename DstType, typename FbxLayerType>
void CopyVertexChannel(FbxMesh* pMesh, FbxLayerType* layer,
		VertexChannel* channel, const IndexArray& polys) {
	for (auto p : polys) {
		NEX_ASSERT(pMesh->GetPolygonSize(p) == 3);

		for (uint32 j = 0; j < 3; ++j) {
			uint32 index = pMesh->GetPolygonVertex(p, j);
			DstType value;

			switch (layer->GetMappingMode()) {
			case FbxGeometryElement::eByControlPoint:
				switch (layer->GetReferenceMode()) {
				case FbxGeometryElement::eDirect: {

					value = Convert<SrcType, DstType>(
							layer->GetDirectArray().GetAt(index));
					channel->PushVertex(&value);
				}
					break;
				case FbxGeometryElement::eIndexToDirect: {
					int32 id = layer->GetIndexArray().GetAt(index);
					value = Convert<SrcType, DstType>(
							layer->GetDirectArray().GetAt(id));
					channel->PushVertex(&value);
				}
					break;
				default:
					break; // other reference modes not shown here!
				}
				break;

			case FbxGeometryElement::eByPolygonVertex:

				switch (layer->GetReferenceMode()) {
				case FbxGeometryElement::eDirect: {
					uint32 vertexId = p * 3 + j;
					value = Convert<SrcType, DstType>(
							layer->GetDirectArray().GetAt(vertexId));
					break;
				}
				case FbxGeometryElement::eIndexToDirect: {
					uint32 vertexId = p * 3 + j;
					int id = layer->GetIndexArray().GetAt(vertexId);
					value = Convert<SrcType, DstType>(
							layer->GetDirectArray().GetAt(vertexId));
				}
					break;
				default:
					break; // other reference modes not shown here!
				}

				break;

			default: // rest doesn't make much sense
				break;
			}

			channel->PushVertex(&value);
		}
	}
}

void FbxMeshLoaderImplv1_0::CreatePrimitiveGroupFrom(FbxSurfaceMaterial* pMtl,
		FbxMesh* pMesh, const IndexArray& polys) {

	// determine vertex channel count
	int uvChannelCount = pMesh->GetElementUVCount();
	int normalCount = pMesh->GetElementNormalCount();
	int biNormalCount = pMesh->GetElementBinormalCount();
	int tangentCount = pMesh->GetElementTangentCount();
	int colorCount = pMesh->GetElementVertexColorCount();

	MeshBuffer* pBuffer = NEX_NEW(MeshBuffer(PrimitiveType::PT_TRI_LIST));
	pBuffer = NEX_NEW(MeshBuffer(PrimitiveType::PT_TRI_LIST));
	pBuffer->AddVertexChannel(COMP_POSITION, 0, COMP_TYPE_FLOAT3);
	if (normalCount > 0)
		pBuffer->AddVertexChannel(COMP_NORMAL, 0, COMP_TYPE_FLOAT3);
	if (tangentCount > 0)
		pBuffer->AddVertexChannel(COMP_TANGENT, 0, COMP_TYPE_FLOAT3);
	if (biNormalCount > 0)
		pBuffer->AddVertexChannel(COMP_BINORMAL, 0, COMP_TYPE_FLOAT3);

	if (colorCount > 0) {
		for (int32 i = 0; i < colorCount; ++i)
			pBuffer->AddVertexChannel(COMP_COLOR, (uint32) i,
					COMP_TYPE_COLOR);
	}

	if (uvChannelCount > 0) {
		for (int32 i = 0; i < uvChannelCount; ++i) {
			pBuffer->AddVertexChannel(COMP_TEXTURE_COORDINATE, (uint32) i,
					COMP_TYPE_FLOAT2);
		}
	}

	pBuffer->ReserveVertexSpace((uint32)polys.size() * 3);
	pBuffer->ReserveIndexSpace((uint32)polys.size() * 3);

	uint32 baseVertexIdx = pBuffer->GetVertexCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();
	VertexChannel* pPosition = pBuffer->GetVertexChannel(
			COMP_POSITION, 0);

	for (auto p : polys) {
		uint32 lPolygonSize = pMesh->GetPolygonSize(p);
		NEX_ASSERT(lPolygonSize == 3);
		for (uint32 j = 0; j < 3; j++) {
			uint32 index = pMesh->GetPolygonVertex(p, j);
			auto &kV = lControlPoints[index];
			float fVertices[3] = { (float) kV.mData[0], (float) kV.mData[1],
					(float) kV.mData[2] };
			pPosition->PushVertex(fVertices);
			pBuffer->PushIndex(baseVertexIdx++);
		}

	}

	if (normalCount > 0) {
		FbxGeometryElementNormal* pFbxNormalLayer = pMesh->GetElementNormal(0);
		VertexChannel* pNormalChannel = pBuffer->GetVertexChannel(COMP_NORMAL,
				0);
		CopyVertexChannel<FbxVector4, Vector3>(pMesh, pFbxNormalLayer,
				pNormalChannel, polys);
	}

	if (tangentCount > 0) {
		FbxGeometryElementTangent* pFbxTangentLayer = pMesh->GetElementTangent(
				0);
		VertexChannel* pTangentChannel = pBuffer->GetVertexChannel(COMP_TANGENT,
				0);
		CopyVertexChannel<FbxVector4, Vector3>(pMesh, pFbxTangentLayer,
				pTangentChannel, polys);
	}

	if (biNormalCount > 0) {
		FbxGeometryElementBinormal* pFbxBinormalLayer =
				pMesh->GetElementBinormal(0);
		VertexChannel* pBinormalChannel = pBuffer->GetVertexChannel(
				COMP_TANGENT, 0);
		CopyVertexChannel<FbxVector4, Vector3>(pMesh, pFbxBinormalLayer,
				pBinormalChannel, polys);
	}

	for (int32 uv = 0; uv < uvChannelCount; ++uv) {
		FbxGeometryElementUV* pFbxUVChannel = pMesh->GetElementUV(uv);
		VertexChannel* pUVChannel = pBuffer->GetVertexChannel(
				COMP_TEXTURE_COORDINATE, (uint32) uv);
		CopyVertexChannel<FbxVector2, Vector2>(pMesh, pFbxUVChannel, pUVChannel,
				polys);
	}

	for (int32 c = 0; c < colorCount; ++c) {
		FbxGeometryElementVertexColor* pFbxColorChannel =
				pMesh->GetElementVertexColor(c);
		VertexChannel* pColorChannel = pBuffer->GetVertexChannel(COMP_COLOR,
				(uint32) c);
		CopyVertexChannel<FbxColor, uint32>(pMesh, pFbxColorChannel,
				pColorChannel, polys);
	}

	pBuffer->RemoveDuplicates();

	bool useSharedVertexSpace = false;
	if (!mSharedBuffer ||
		 mSharedBuffer->GetVertexSignature() == pBuffer->GetVertexSignature()) {
		useSharedVertexSpace = true;
	}

	MeshElement kElement;
	kElement.mVertexCount = pBuffer->GetVertexCount();
	kElement.mIndexCount = pBuffer->GetIndexCount();
	kElement.mMaterial = pMtl;
	kElement.mBounds = pBuffer->ComputeBounds();

	if (useSharedVertexSpace && mSharedBuffer) {
		kElement.mStartVertex = mSharedBuffer->GetVertexCount();
		kElement.mStartIndex = mSharedBuffer->GetIndexCount();
		kElement.mMesh = nullptr;
		mSharedBuffer->MergeBuffer(*pBuffer);
		NEX_DELETE(pBuffer);
	} else {
		kElement.mStartVertex = 0;
		kElement.mStartIndex = 0;
		kElement.mMesh = pBuffer;
		if (useSharedVertexSpace)
			mSharedBuffer = pBuffer;
	}

	mElements.push_back(kElement);
}

void FbxMeshLoaderImplv1_0::BuildMesh(MeshTemplate::MeshBuilder* pMesh) {

	pMesh->SetBounds(mFullBounds);
	pMesh->SetSharedBuffer(mSharedBuffer);
	for(auto &e : mElements) {
		MeshTemplate::PrimitiveGroup& pg = pMesh->AddPrimitiveGroup();
		pg.buffer = e.mMesh == mSharedBuffer ? nullptr : e.mMesh;
		pg.indexCount = e.mIndexCount;
		pg.startIndex = e.mStartIndex;
		pg.vertexCount = e.mVertexCount;
		pg.startVertex = e.mStartVertex;
		pg.bounds = e.mBounds;
		pg.material = CreateMaterial(e.mMaterial, pMesh->GetAssetLocator());

		if (pg.material)
			pMesh->GetMetaInfo().AddDependency(pg.material);
	}

	mElements.clear();
	mSharedBuffer = nullptr;
}

MaterialTemplatePtr FbxMeshLoaderImplv1_0::CreateMaterial(FbxSurfaceMaterial* pFbxMat, const URL& kAssetLoc) {
	MaterialTemplate::ID id(
		NamedObject::AsyncStringID(pFbxMat->GetName()),
		StringUtils::DefaultID,
		StringUtils::DefaultID);
	MaterialTemplatePtr material;
	const FbxProperty lProperty = pFbxMat->FindProperty("NFX_Material");
	if (lProperty.IsValid()) {
		FbxString kMtlPath = lProperty.Get<FbxString>();
		String url = (const char*)kMtlPath;
		URL path = URL(url);
		material = MaterialTemplate::Traits::Instance(id, path);
		return material;
	} else {
		material = MaterialTemplate::Traits::Instance(id);
	}

	if (material->AsyncIsCreated()) {
		InputStreamPtr dummy;
		FbxMaterialLoaderImplv1_0* manualLoader = NEX_NEW(FbxMaterialLoaderImplv1_0(pFbxMat, kAssetLoc));
		StreamInfo streamInfo(manualLoader, dummy);
		if(!material->AsyncSetLoadInfo(streamInfo))
			NEX_DELETE(manualLoader);
	}

	return material;
}

}
