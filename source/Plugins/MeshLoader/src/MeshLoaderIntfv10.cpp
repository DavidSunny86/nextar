/* 
 * File:   MeshLoaderImplv1_0.cpp
 * Author: obhi
 * 
 * Created on May 22, 2011, 9:55 AM
 */
#include "NexHeaders.h"
#include "MeshLoaderIntfv10.h"
#include "BufferManager.h"
#include "VertexElement.h"
#include "GenericStreams.h"

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
		MCID_MESH_HEADER,
		MCID_END
	};

	MaterialAsset* MeshLoaderImplv1_0::ReadMaterialData(MeshAsset::StreamRequest* mesh, InputSerializer& ser) {
		StringID factory, name, materialGroup;
		String path;
		ser >> factory >> name >> materialGroup >> path;
		MaterialAsset* mtl = 0;
		URL location(path);
		Asset::Factory* managerPtr = ComponentFactoryArchive::Instance().AsyncFindFactory(MaterialAsset::CLASS_ID, name);
		if (managerPtr) {
			// create a new request for material load??
			mtl = MaterialAsset::Instance(managerPtr, name, location);
		}
		return mtl;
	}

	void MeshLoaderImplv1_0::FindStreamVertexElements(MeshAsset::StreamRequest* mesh,
			const VertexElement*& start, const VertexElement*& end,
			uint16 streamIndex, const VertexElement* elem, uint16 numVertexElements) {
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

	void MeshLoaderImplv1_0::ReadVertexElementData(MeshAsset::StreamRequest* request,
			InputSerializer& ser, VertexElement*& vertexElements,
			uint16& vertexElementCount) {

		Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
		if (vertexElements)
			NEX_DELETE vertexElements;
		ser >> (vertexElementCount);
		if (vertexElementCount >= VertexElement::MAX_VERTEX_ELEMENT) {
			Error(String("Too many vertex elements: ") + mesh->GetName());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}

		vertexElements = NEX_NEW VertexElement[vertexElementCount];
		uint32 i = 0;
		while (i < vertexElementCount) {
			VertexElement element;
			ser >> (element.desc.semantic.instanceData)
				>> (element.desc.semantic.semantic)
				>> (element.desc.semantic.semanticIndex)
				>> (element.desc.semantic.type)
				>> (element.desc.offset)
				>> (element.stepRate)
				>> (element.desc.stride)
				>> (element.streamIndex);
			/** todo check sanity */
			vertexElements[i++] = element;
		}

		if (i != vertexElementCount) {
			Error(String("Failed to read vertex elements: ") + mesh->GetName());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}

		/* Sort based on stream index */
		std::sort(vertexElements, vertexElements + vertexElementCount);
	}

	void MeshLoaderImplv1_0::ReadVertexBufferData(MeshAsset::StreamRequest* request,
			InputSerializer& ser, MeshVertexData*& vertexData) {

		Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
		uint16 streamIndex;
		uint32 size;
		uint32 vertexCount;

		ser >> size
			>> vertexCount
			>> streamIndex;

		if (streamIndex >= vertexData->numVertexBuffers) {
			Error(String("Stream index out of bounds: ") + mesh->GetName());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}

		ByteStream& stream = request->AddVertexBuffer();
		stream.resize(size);
		uint8* vertexBuffer = &stream[0];

		ser >> InputSerializer::ByteArray(vertexBuffer, size);
		if (!InputSerializer::IsEndianCorrected()) {
			const VertexElement* start, *end;
			FindStreamVertexElements(request, start, end, streamIndex,
					vertexData->vertexElements, vertexData->numVertexElements);
			MeshAsset::Loader::EndianFlip(vertexBuffer, start, end, vertexCount);
		}
	}

	MeshVertexData* MeshLoaderImplv1_0::ReadVertexData(
			MeshAsset::StreamRequest* request,
			InputSerializer& ser,
			VertexElement* vertexElements, uint16 vertexElementCount) {

		Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
		MeshVertexData* vertexData = NEX_NEW MeshVertexData;
		// read some headers
		ser >> vertexData->numVertexBuffers >> vertexData->vertexCount;

		if (vertexData->numVertexBuffers >= MAX_STREAM_COUNT) {
			Error(String("Too many vertex buffers: ") + mesh->GetName());
			NEX_DELETE vertexData;
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}

		// the first chunk should be vertex elements, if not shared
		InputSerializer::Chunk chunk;
		ser >> chunk;
		if (chunk.first.first != MCID_VERTEX_ELEMENT_DATA && !vertexElements) {
			Error(String("Vertex element data missing: ") + mesh->GetName());
			NEX_DELETE vertexData;
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
		}

		// read vertex elements if availabe, hence not shared
		if (chunk.first.first == MCID_VERTEX_ELEMENT_DATA) {
			// read
			ReadVertexElementData(request, ser, vertexData->vertexElements,
					vertexData->numVertexElements);
		} else {
			vertexData->vertexElements = vertexElements;
			vertexData->numVertexElements = vertexElementCount;
		}

		size_t i = 0;
		do {
			switch(chunk.first.first) {
			case MCID_VERTEX_BUFFER_DATA:
				ReadVertexBufferData(request, ser, vertexData);
				i++;
				break;
			}
		}while(InputSerializer::IsValid(chunk) && (i < vertexData->numVertexBuffers));

		return vertexData;
	}

	MeshIndexData* MeshLoaderImplv1_0::ReadIndexData(MeshAsset::StreamRequest* request, InputSerializer& ser) {
		Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
		MeshIndexData* indexData = NEX_NEW MeshIndexData;

		uint32 size;
		ser >> size >> indexData->indexCount >> indexData->twoBytePerElement;
		ByteStream& stream = request->AddIndexBuffer();
		stream.resize(size);
		uint8* indexBuffer = &stream[0];

		if (indexData->twoBytePerElement)
			ser >> InputSerializer::UShortArray(static_cast<uint16*>((indexBuffer)), size/2);
		else
			ser >> InputSerializer::UIntArray(static_cast<uint32*>((indexBuffer)), size/4);

		return indexData;
	}

	BoundsInfo MeshLoaderImplv1_0::ReadBoundsInfo(MeshAsset::StreamRequest* mesh, InputSerializer& ser) {
		BoundsInfo bv;
		float radius;
		float extendsAndCenter[6];

		ser >> radius >> InputSerializer::FloatArray(extendsAndCenter, 6);
		bv.linearRadius = radius;
		bv.origRadius = Vector3(extendsAndCenter);
		bv.origCenter = Vector3(extendsAndCenter + 3);
		return bv;
	}

	void MeshLoaderImplv1_0::ReadSubMesh(MeshAsset::StreamRequest* request, InputSerializer& ser) {
		Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
		uint32 subMesh = request->AddPrimitiveGroup();
		// right now all buffers are shared by subMesh
		InputSerializer::Chunk chunk(MARKER_INVALID_CHUNK, 0);

		MaterialAssetPtr mtl;
		MeshVertexData* vertexData = 0;
		MeshIndexData* indexData = 0;
		VertexElement* ve = 0;
		uint16 vec = 0;
		uint32 start, count;

		do {
			ser >> chunk;
			if (InputSerializer::IsValid(chunk)) {
				switch (chunk.first.first) {
				case MCID_VERTEX_DATA:
					vertexData = request->GetSharedVertexData();
					if (vertexData) {
						ve = vertexData->vertexElements;
						vec = vertexData->numVertexElements;
					}
					vertexData = ReadVertexData(request, ser, ve, vec);
					request->SetPrimitiveVertexData(subMesh, vertexData, 0, vertexData->vertexCount);
					break;
				case MCID_INDEX_DATA:
					indexData = ReadIndexData(request, ser);
					request->SetPrimitiveIndexData(subMesh, indexData, 0, indexData->indexCount);
					break;
				case MCID_SHARED_VERTEX_DATA:
					// just read the count and start
					ser >> start
						>> count;
					request->SetPrimitiveVertexData(subMesh, 0, start, count);
					break;
				case MCID_SHARED_INDEX_DATA:
					// just read the count and start
					ser >> start
						>> count;
					request->SetPrimitiveIndexData(subMesh, 0, start, count);
					break;
				case MCID_MATERIAL_DATA:
					mtl = ReadMaterialData(request, ser);
					request->SetPrimitiveMaterial(subMesh, mtl);
					break;
				case MCID_BOUNDS_INFO:
					request->SetPrimitiveBoundsInfo(subMesh, ReadBoundsInfo(request, ser));
					break;
				case MCID_SUBMESH_END:
					return;
				}
			} else {
				Error(String("File is corrupt: ") + mesh->GetName());
				NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
			}
		} while (!ser.IsEndOfStream());
	}

	void MeshLoaderImplv1_0::ReadMeshHeader(MeshAsset::StreamRequest* mesh, InputSerializer& ser) {
		uint32 numSubMesh;
		ser >> numSubMesh;
		mesh->SetPrimitiveGroupCount(numSubMesh);
	}

	void MeshLoaderImplv1_0::ReadMeshChunk(MeshAsset::StreamRequest* request, InputSerializer& ser) {
		/* check if data is shared */
		Asset* mesh = static_cast<Asset*>(request->GetStreamedObject());
		InputSerializer::Chunk chunk;
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
								ReadVertexData(request, ser, 0, 0));
					else {
						if (!headersRead)
							Error("Vertex data should appear after mesh header: " + mesh->GetName());
						if (subMeshesRead)
							Error("Vertex data should appear before primitive groups: " + mesh->GetName());
					}
					break;
				case MCID_INDEX_DATA:
					if (headersRead && !subMeshesRead)
						request->SetSharedIndexData(ReadIndexData(request, ser));
					else {
						if (!headersRead)
							Error("Index data should appear after mesh header: " + mesh->GetName());
						if (subMeshesRead)
							Error("Index data should appear before primitive groups: " + mesh->GetName());
					}
					break;
				case MCID_MATERIAL_DATA:
					mtl = Bind(ReadMaterialData(request, ser));
					request->SetSharedMaterial(mtl);
					break;
				case MCID_SUBMESH_DATA:
					if (headersRead)
						ReadSubMesh(request, ser);
					else
						Error("Primitive groups should appear after mesh header: " + mesh->GetName());
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
				Error(String("File is corrupt: ") + mesh->GetName());
				NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOAD_ASSET);
			}
		} while (!ser.IsEndOfStream());
	}

	MeshLoaderImplv1_0::MeshLoaderImplv1_0() {
	}

	MeshLoaderImplv1_0::~MeshLoaderImplv1_0() {
	}

	void MeshLoaderImplv1_0::Load(InputSerializer& streamPtr, MeshAsset::Loader& loader) {
		MeshAsset::StreamRequest* request = static_cast<MeshAsset::StreamRequest*>(loader.GetRequestPtr());
		ReadMeshChunk(request, streamPtr);
	}
}
