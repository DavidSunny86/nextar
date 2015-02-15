/*
 * MeshSaverImplv10.cpp
 *
 *  Created on: 03-Oct-2014
 *      Author: obhi
 */
#include <NexProjectModel.h>
#include <MeshSaverImplv10.h>

namespace nextar {

MeshSaverImplv1_0 MeshSaverImplv1_0::saver;

enum MeshFileMagic {
	FILE_MAGIC_MESH = 0xff811901
};

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

MeshSaverImplv1_0::MeshSaverImplv1_0() {
	// TODO Auto-generated constructor stub

}

MeshSaverImplv1_0::~MeshSaverImplv1_0() {
	// TODO Auto-generated destructor stub
}

void MeshSaverImplv1_0::WriteVersion(OutputStreamPtr& out) {
	OutputSerializer ser(out);
	uint32 meshMagic = FILE_MAGIC_MESH;
	VersionID ver = NEX_MAKE_VERSION(1, 0, 0);
	String versionInfo = Convert::ToVersionString(ver);
	ser << meshMagic << versionInfo;
}

void MeshSaverImplv1_0::WriteMeshHeader(MeshTemplate* templ,
		ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_MESH_HEADER);
	uint32 numSubMesh = templ->GetNumPrimitiveGroups();
	// header << numSubMesh
	ser << numSubMesh;
	outStream.EndChunk();
}

void MeshSaverImplv1_0::WriteElementData(MeshBuffer* buffer,
		ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_VERTEX_ELEMENT_DATA);
	VertexLayoutInfo layout;
	buffer->GetVertexLayout(layout);
	uint16 layoutType = layout.layoutType;
	ser << layoutType;
	if (layoutType == CUSTOM_LAYOUT) {
		uint8 vertexElementCount = (uint8)layout.vertexElements.size();
		ser  << vertexElementCount;
		for(auto& element : layout.vertexElements) {
			ser << (element.desc.semantic.instanceData)
					<< (uint16)(element.desc.semantic.semantic)
					<< (element.desc.semantic.semanticIndex)
					<< (uint16)(element.desc.semantic.type) << (element.desc.offset)
					<< (element.stepRate)
					<< (element.streamIndex);
		}
	}
	outStream.EndChunk();
}

void MeshSaverImplv1_0::WriteIndexData(MeshBuffer* buffer,
		ChunkOutputStream& outStream) {
	ByteStream indices;
	buffer->GetOptimizedIndices(indices);
	OutputSerializer& ser = outStream.BeginChunk(MCID_INDEX_DATA);
	uint32 size = (uint32)indices.size();
	uint32 count = buffer->GetIndexCount();
	ser << size << count;
	if ((size / count) == 4) {
		OutputSerializer::UIntArray arr(reinterpret_cast<uint32*>(indices.data()), count);
		ser	 << arr;
	} else {
		OutputSerializer::UShortArray arr(reinterpret_cast<uint16*>(indices.data()), count);
		ser	 << arr;
	}
	outStream.EndChunk();
}

void MeshSaverImplv1_0::WriteVertexBufferData(MeshBuffer* buffer,
		uint32 numVB,
		ChunkOutputStream& outStream) {
	ByteStream bytes;
	for (uint32 i = 0; i < numVB; ++i) {
		OutputSerializer& ser = outStream.BeginChunk(MCID_VERTEX_BUFFER_DATA);

		uint32 vertexCount = buffer->GetVertexCount();
		uint32 vertexStride = buffer->GetVertexStride(i);
		uint32 bufferSize =  vertexCount * vertexStride;
		ser << bufferSize << vertexCount << i << vertexStride;
		buffer->GetVertices(i, bytes);
		OutputSerializer::UByteArray arr(bytes.data(), (uint32)bytes.size());
		NEX_ASSERT(arr.second == bufferSize);
		ser << arr;
		outStream.EndChunk();
	}
}

void MeshSaverImplv1_0::WriteMaterialData(const MaterialTemplatePtr& material,
		ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_MATERIAL_DATA);
	MaterialAsset::ID id = material->GetMaterialID();
	URL location = material->GetAssetLocator();
	ser << id << location;
	outStream.EndChunk();
}

void MeshSaverImplv1_0::WriteVertexData(MeshBuffer* buffer,
		ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_VERTEX_DATA);
	uint32 numVertexBuffer = buffer->GetVertexBufferCount();
	uint32 vertexCount = buffer->GetVertexCount();
	ser << numVertexBuffer << vertexCount;
	WriteElementData(buffer, outStream);
	WriteVertexBufferData(buffer, numVertexBuffer, outStream);
	outStream.EndChunk();
}

void MeshSaverImplv1_0::WriteBoundsInfo(const BoundsInfo& bounds,
		ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_BOUNDS_INFO);
	ser << bounds.radius
		<< bounds.center.x
		<< bounds.center.y
		<< bounds.center.z
		<< bounds.extends.x
		<< bounds.extends.y
		<< bounds.extends.z;
	outStream.EndChunk();
}

void MeshSaverImplv1_0::Save(OutputStreamPtr& out,
		AssetSaver& saver) {

	try {
		MeshTemplate* mesh = static_cast<MeshTemplate*>(
					saver.GetRequestPtr()->GetStreamedObject());

		WriteVersion(out);

		ChunkOutputStream stream(out);
		WriteMeshHeader(mesh, stream);
		WriteBoundsInfo(mesh->GetBounds(), stream);
		MeshBuffer* buffer = mesh->GetSharedMeshBuffer();
		MaterialTemplatePtr material = mesh->GetSharedMaterial();
		if (buffer) {
			WriteVertexData(buffer, stream);
			WriteIndexData(buffer, stream);
		}

		if (material) {
			WriteMaterialData(material, stream);
		}

		OutputSerializer& ser = stream.BeginChunk(MCID_SUBMESH_DATA);
		uint32 numPrim = mesh->GetNumPrimitiveGroups();
		for (uint32 i = 0; i < numPrim; ++i)
			WriteSubMeshData(mesh, mesh->GetPrimitive(i), stream);
		stream.EndChunk();
	} catch (const GracefulErrorExcept& e) {
		saver.GetRequestPtr()->SetCompleted(false);
		NEX_THROW(e);
	}

	saver.GetRequestPtr()->SetCompleted(true);
}

void MeshSaverImplv1_0::WriteSubMeshData(MeshTemplate* templ,
		const MeshTemplate::PrimitiveGroup& gr,
		ChunkOutputStream& outStream) {

	bool sharedMeshBuffer = false;
	MeshBuffer* buffer = templ->GetSharedMeshBuffer();
	if (!gr.buffer || gr.buffer == buffer) {
		sharedMeshBuffer = true;
	} else
		buffer = gr.buffer;

	{
		uint8 primType = (uint8)buffer->GetPrimitiveType();
		OutputSerializer& ser = outStream.BeginChunk(MCID_SUBMESH_INFO);
		ser << primType;
		outStream.EndChunk();
	}

	if (sharedMeshBuffer) {
		{
			uint32 start = gr.startVertex;
			uint32 count = gr.vertexCount;
			OutputSerializer& ser = outStream.BeginChunk(MCID_SHARED_VERTEX_DATA);
			ser << start << count;
			outStream.EndChunk();
		}
		{
			uint32 start = gr.startIndex;
			uint32 count = gr.indexCount;
			OutputSerializer& ser = outStream.BeginChunk(MCID_SHARED_INDEX_DATA);
			ser << start << count;
			outStream.EndChunk();
		}
	} else {
		WriteVertexData(buffer, outStream);
		WriteIndexData(buffer, outStream);
	}

	WriteBoundsInfo(gr.bounds, outStream);
	if (gr.material && gr.material != templ->GetSharedMaterial()) {
		WriteMaterialData(gr.material, outStream);
	}

	{
		OutputSerializer& ser = outStream.BeginChunk(MCID_SUBMESH_END);
		outStream.EndChunk();
	}
}

} /* namespace nextar */
