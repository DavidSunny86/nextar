/* 
 * File:   MeshLoaderImplv1_0.cpp
 * Author: obhi
 * 
 * Created on May 22, 2011, 9:55 AM
 */
#include <BaseHeaders.h>
#include <MeshLoaderIntfv10.h>
#include <BufferManager.h>
#include <VertexElement.h>
#include <GenericStreams.h>
#include <ComponentFactoryArchive.h>

namespace MeshLoader {

enum MeshChunkID {

	MCID_VERTEX_DATA = 0x9111,
	MCID_BOUNDS_INFO,
	MCID_SHARED_VERTEX_DATA,

	MCID_VERTEX_BUFFER_DATA,
	MCID_VERTEX_ELEMENT_DATA,
	MCID_INDEX_DATA,
	MCID_SHARED_INDEX_DATA,
	MCID_MATERIAL_DATA,
	MCID_MATERIAL_SCRIPT,
	MCID_SUBMESH_DATA,
	MCID_SUBMESH_END,
	MCID_SUBMESH_INFO,
	MCID_MESH_HEADER,
	MCID_END
};

MaterialAssetPtr MeshLoaderIntfv1_0::ReadMaterialData(
		MeshAsset::StreamRequest* mesh, InputSerializer& ser) {
	MaterialAsset::ID id;
	URL path;
	ser >> id >> path;
	return MaterialAsset::Traits::Instance(id, path);
}

void MeshLoaderIntfv1_0::FindStreamVertexElements(
		MeshAsset::StreamRequest* mesh, const VertexElement*& start,
		const VertexElement*& end, uint16 streamIndex,
		const VertexElement* elem, uint16 numVertexElements) {
	start = 0;
	end = 0;
	const VertexElement* en = elem + numVertexElements;
	while (elem != en) {
		if ((*elem).streamIndex == streamIndex) {
			start = elem;
			break;
		}
		elem++;
	}

	if (!start)
		return;

	while (elem != en) {
		if ((*elem).streamIndex != streamIndex) {
			break;
		}
		end = elem;
		elem++;
	}

	end++;
}

void MeshLoaderIntfv1_0::ReadVertexElementData(
		MeshAsset::StreamRequest* request, InputSerializer& ser,
		uint16& layoutType, uint8& vertexElementCount, VertexElement*& vertexElements) {

	Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
	if (vertexElements)
		NEX_DELETE(vertexElements);
	ser >> layoutType;

	if (layoutType != (uint16)VertexLayoutType::CUSTOM_LAYOUT) {
		vertexElementCount = 0;
		vertexElements = 0;
		if(layoutType >= (uint16)VertexLayoutType::VERTEX_LAYOUT_COUNT) {
			Error(String("Layout not supported: ") + request->GetName());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}
		return;
	}
	ser >> (vertexElementCount);
	if (vertexElementCount >= VertexElement::MAX_VERTEX_ELEMENT) {
		Error(String("Too many vertex elements: ") + request->GetName());
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}

	vertexElements = NEX_NEW(VertexElement[vertexElementCount]);
	uint32 i = 0;
	while (i < vertexElementCount) {
		VertexElement element;
		ser >> (element.desc.semantic.instanceData)
				>> (element.desc.semantic.semantic)
				>> (element.desc.semantic.semanticIndex)
				>> (element.desc.semantic.type) >> (element.desc.offset)
				>> (element.stepRate)
				>> (element.streamIndex);
		/** todo check sanity */
		vertexElements[i++] = element;
	}

	if (i != vertexElementCount) {
		Error(String("Failed to read vertex elements: ") + request->GetName());
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}

	/* Sort based on stream index */
	std::sort(vertexElements, vertexElements + vertexElementCount);
}

void MeshLoaderIntfv1_0::ReadVertexBufferData(MeshAsset::StreamRequest* request,
		InputSerializer& ser, MeshVertexData*& vertexData) {

	Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
	uint16 streamIndex;
	uint32 size;
	uint32 vertexCount;
	uint32 vertexStride;

	ser >> size >> vertexCount >> streamIndex >> vertexStride;

	auto& stream = request->AddVertexBuffer();
	stream.buffer.resize(size);
	uint8* vertexBuffer = stream.buffer.data();
	stream.stride = vertexStride;

	InputSerializer::UByteArray readBuffer(vertexBuffer, size);
	ser >> readBuffer;
	if (!InputSerializer::IsEndianCorrected()) {
		const VertexElement* vertexElements = vertexData->vertexElements;
		uint32 numVertexElements = vertexData->numVertexElements;
		const VertexElement* start, *end;
		if (vertexData->layoutType != VertexLayoutType::CUSTOM_LAYOUT) {
			VertexLayout::GetCustomLayoutElements(vertexData->layoutType,
					vertexElements, numVertexElements);
		}
		FindStreamVertexElements(request, start, end, streamIndex,
				vertexElements, (uint16)numVertexElements);
		MeshAsset::EndianFlip(vertexBuffer, start, end, vertexCount, vertexStride);
	}
}

MeshVertexData* MeshLoaderIntfv1_0::ReadVertexData(
		MeshAsset::StreamRequest* request, InputSerializer& ser,
		VertexElement* vertexElements, uint16 vertexElementCount, VertexLayoutType type) {

	Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
	MeshVertexData* vertexData = NEX_NEW(MeshVertexData);
	// read some headers
	uint32 numVertexBuffers;
	ser >> numVertexBuffers >> vertexData->vertexCount;
	
	if (numVertexBuffers >= MAX_STREAM_COUNT) {
		Error(String("Too many vertex buffers: ") + request->GetName());
		NEX_DELETE(vertexData);
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}

	vertexData->numVertexBuffers = numVertexBuffers;
	// the first chunk should be vertex elements, if not shared
	InputSerializer::Chunk chunk = InputSerializer::Invalid;
	ser >> chunk;
	if (chunk.first.first != MCID_VERTEX_ELEMENT_DATA && !vertexElements 
		&& type == VertexLayoutType::CUSTOM_LAYOUT) {
		Error(String("Vertex element data missing: ") + request->GetName());
		NEX_DELETE(vertexData);
		NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
	}

	// read vertex elements if availabe, hence not shared
	if (chunk.first.first == MCID_VERTEX_ELEMENT_DATA) {
		// read
		uint16 layoutType;
		ReadVertexElementData(request, ser,
				layoutType,
				vertexData->numVertexElements, vertexData->vertexElements);
		vertexData->layoutType = (VertexLayoutType)layoutType;
	} else {
		vertexData->layoutType = type;
		vertexData->vertexElements = vertexElements;
		vertexData->numVertexElements = (uint8)vertexElementCount;
	}

	size_t i = 0;
	do {
		ser >> chunk;
		switch (chunk.first.first) {
		case MCID_VERTEX_BUFFER_DATA:
			ReadVertexBufferData(request, ser, vertexData);
			i++;
			break;
		}
	} while (InputSerializer::IsValid(chunk)
			&& (i < numVertexBuffers));

	return vertexData;
}

MeshIndexData* MeshLoaderIntfv1_0::ReadIndexData(
		MeshAsset::StreamRequest* request, InputSerializer& ser) {
	Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
	MeshIndexData* indexData = NEX_NEW(MeshIndexData);

	uint32 size;
	ser >> size >> indexData->indexCount;

	auto& stream = request->AddIndexBuffer();
	stream.stride = (size / indexData->indexCount);
	stream.buffer.resize(size);
	uint8* indexBuffer = stream.buffer.data();

	if (stream.stride==2) {
		InputSerializer::UShortArray dataBuff(
				reinterpret_cast<uint16*>(indexBuffer), size / 2);
		ser >> dataBuff;
	} else {
		NEX_ASSERT(stream.stride==4);
		InputSerializer::UIntArray dataBuff(
				reinterpret_cast<uint32*>(indexBuffer), size / 4);
		ser >> dataBuff;
	}

	return indexData;
}

BoundsInfo MeshLoaderIntfv1_0::ReadBoundsInfo(MeshAsset::StreamRequest* mesh,
		InputSerializer& ser) {
	BoundsInfo bv;
	float radius;
	float extendsAndCenter[6];
	InputSerializer::FloatArray extends(extendsAndCenter, 6);
	ser >> radius >> extends;
	bv.radius = radius;
	bv.center = Vector3(extendsAndCenter);
	bv.extends = Vector3(extendsAndCenter + 3);
	return bv;
}

void MeshLoaderIntfv1_0::ReadSubMesh(MeshAsset::StreamRequest* request,
		InputSerializer& ser) {
	Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
	uint32 subMesh = request->AddPrimitiveGroup();
	// right now all buffers are shared by subMesh
	InputSerializer::Chunk chunk = InputSerializer::Invalid;

	MaterialAssetPtr mtl;
	MeshVertexData* vertexData = 0;
	MeshIndexData* indexData = 0;
	VertexElement* ve = 0;
	uint16 vec = 0;
	VertexLayoutType vetype = VertexLayoutType::CUSTOM_LAYOUT;
	uint32 start, count;
	uint8 primType;

	do {
		ser >> chunk;
		if (InputSerializer::IsValid(chunk)) {
			switch (chunk.first.first) {
			case MCID_SUBMESH_INFO:
				ser >> primType;
				request->SetPrimitiveType(subMesh, (PrimitiveType)primType);
				break;
			case MCID_VERTEX_DATA:
				vertexData = request->GetSharedVertexData();
				if (vertexData) {
					vetype = vertexData->layoutType;
					ve = vertexData->vertexElements;
					vec = vertexData->numVertexElements;
				}
				vertexData = ReadVertexData(request, ser, ve, vec, vetype);
				request->SetPrimitiveVertexData(subMesh, vertexData, 0,
						vertexData->vertexCount);
				break;
			case MCID_INDEX_DATA:
				indexData = ReadIndexData(request, ser);
				request->SetPrimitiveIndexData(subMesh, indexData, 0,
						indexData->indexCount);
				break;
			case MCID_SHARED_VERTEX_DATA:
				// just read the count and start
				ser >> start >> count;
				request->SetPrimitiveVertexData(subMesh, 0, start, count);
				break;
			case MCID_SHARED_INDEX_DATA:
				// just read the count and start
				ser >> start >> count;
				request->SetPrimitiveIndexData(subMesh, 0, start, count);
				break;
			case MCID_MATERIAL_DATA:
				mtl = ReadMaterialData(request, ser);
				request->SetPrimitiveMaterial(subMesh, mtl);
				break;
			case MCID_BOUNDS_INFO:
				request->SetPrimitiveBoundsInfo(subMesh,
						ReadBoundsInfo(request, ser));
				break;
			case MCID_SUBMESH_END:
				return;
			}
		} else {
			Error(String("File is corrupt: ") + request->GetName());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}
	} while (!ser.IsEndOfStream());
}

void MeshLoaderIntfv1_0::ReadMeshHeader(MeshAsset::StreamRequest* mesh,
		InputSerializer& ser) {
	uint32 numSubMesh;
	ser >> numSubMesh;
	mesh->SetPrimitiveGroupCount(numSubMesh);
}

void MeshLoaderIntfv1_0::ReadMeshChunk(MeshAsset::StreamRequest* request,
		InputSerializer& ser) {
	/* check if data is shared */
	Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
	InputSerializer::Chunk chunk = InputSerializer::Invalid;
	bool headersRead = false;
	bool subMeshesRead = false;
	MaterialAssetPtr mtl;

	do {
		ser >> chunk;
		if (InputSerializer::IsValid(chunk)) {
			switch (chunk.first.first) {
			case MCID_VERTEX_DATA:
				if (headersRead && !subMeshesRead)
					request->SetSharedVertexData(
							ReadVertexData(request, ser, 0, 0, VertexLayoutType::CUSTOM_LAYOUT));
				else {
					if (!headersRead)
						Error(
								"Vertex data should appear after mesh header: "
										+ request->GetName());
					if (subMeshesRead)
						Error(
								"Vertex data should appear before primitive groups: "
										+ request->GetName());
				}
				break;
			case MCID_INDEX_DATA:
				if (headersRead && !subMeshesRead)
					request->SetSharedIndexData(ReadIndexData(request, ser));
				else {
					if (!headersRead)
						Error(
								"Index data should appear after mesh header: "
										+ request->GetName());
					if (subMeshesRead)
						Error(
								"Index data should appear before primitive groups: "
										+ request->GetName());
				}
				break;
			case MCID_MATERIAL_DATA:
				mtl = ReadMaterialData(request, ser);
				request->SetSharedMaterial(mtl);
				break;
			case MCID_SUBMESH_DATA:
				if (headersRead)
					ReadSubMesh(request, ser);
				else
					Error(
							"Primitive groups should appear after mesh header: "
									+ request->GetName());
				subMeshesRead = true;
				break;
			case MCID_BOUNDS_INFO:
				request->SetBoundsInfo(ReadBoundsInfo(request, ser));
				break;
			case MCID_MESH_HEADER:
				ReadMeshHeader(request, ser);
				headersRead = true;
				break;
			case MCID_END:
				return;
			}
		} else {
			Error(String("File is corrupt: ") + request->GetName());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}
	} while (!ser.IsEndOfStream());
}

MeshLoaderIntfv1_0::MeshLoaderIntfv1_0() {
}

MeshLoaderIntfv1_0::~MeshLoaderIntfv1_0() {
}

void MeshLoaderIntfv1_0::Load(InputSerializer& streamPtr, AssetLoader& loader) {
	MeshAsset::StreamRequest* request =
			static_cast<MeshAsset::StreamRequest*>(loader.GetRequestPtr());
	ReadMeshChunk(request, streamPtr);
}
}
