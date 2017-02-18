/*
 * Main.cpp
 *
 *  Created on: 04-Mar-2014
 *      Author: obhi
 */
#include <BaseHeaders.h>

NEX_IMPLEMENT_PLATFORM_APP()
;

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

using namespace nextar;

void WriteMeshHeader(ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_MESH_HEADER);
	uint32 numSubMesh = 1;
	// header << numSubMesh
	ser << numSubMesh;
	outStream.EndChunk();
}

void WriteElementData(ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_VERTEX_ELEMENT_DATA);
	uint16 layoutType = 1; // Position and Normal
	ser << layoutType;
	outStream.EndChunk();
}

void WriteVertexBufferData(ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_VERTEX_BUFFER_DATA);
	uint16 streamIndex = 0;
	uint32 vertexCount = 8;
	uint32 vertexStride = 4*3*2;
	uint32 size = 8 * vertexStride;
	float vertexData[] = {
		// position, normal
		1.000000f, 1.000000f, -1.000000f, 0.333333f, 0.666667f, -0.666667f, 
		-1.000000f, 1.000000f, -1.000000f, -0.816497f, 0.408248f, -0.408248f, 
		-1.000000f, 1.000000f, 1.000000f, -0.333333f, 0.666667f, 0.666667f, 
		1.000000f, 1.000000f, 1.000000f, 0.816497f, 0.408248f, 0.408248f, 
		1.000000f, -1.000000f, -1.000000f, 0.666667f, -0.666667f, -0.333333f, 
		-1.000000f, -1.000000f, -1.000000f, -0.408248f, -0.408248f, -0.816497f, 
		-1.000000f, -1.000000f, 1.000000f, -0.666667f, -0.666667f, 0.333333f, 
		1.000000f, -1.000000f, 1.000000f, 0.408248f, -0.408248f, 0.816497f, 
	};

	OutputSerializer::FloatArray writeBuffer(vertexData, vertexCount*3*2);
	ser << size << vertexCount << streamIndex << vertexStride;
	ser << writeBuffer;
	outStream.EndChunk();
}

void WriteIndexData(ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_INDEX_DATA);
	uint32 indexCount = 36;
	uint32 size = indexCount * sizeof(uint16);
	ser << size << indexCount;
	uint16 indexData[] = {
		0,1,2,
		0,2,3,
		0,4,5,
		0,5,1,
		1,5,6,
		1,6,2,
		2,6,7,
		2,7,3,
		3,7,4,
		3,4,0,
		4,7,6,
		4,6,5,
	};
	OutputSerializer::UShortArray writeBuffer(indexData, 36);
	outStream.EndChunk();
}

void WriteVertexData(ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_VERTEX_DATA);
	uint32 numVertexBuffer = 1;
	uint32 vertexCount = 8;
	ser << numVertexBuffer << vertexCount;
	WriteElementData(outStream);
	WriteVertexBufferData(outStream);
	outStream.EndChunk();
}

void WriteBoundsInfo(ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_BOUNDS_INFO);
	float radius = 3.5f;
	float extendsAndCenter[6] = {
		0, 0, 0,
		1, 1, 1,
	};
	OutputSerializer::FloatArray extends(extendsAndCenter, 6);
	ser << radius << extends;
	outStream.EndChunk();
}

void WriteMaterialData(ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_MATERIAL_DATA);
	StringID name(StringUtils::GetStringID("Box")), factory(StringUtils::DefaultID), group(StringUtils::DefaultID);
	URL path("{EngineData}/Materials/Box.mtl");
	ser << name << factory << group << path;
	outStream.EndChunk();
}

void WriteSharedVertexData(ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_SHARED_VERTEX_DATA);
	uint32 start = 0;
	uint32 count = 8;
	ser << start << count;
	outStream.EndChunk();
}

void WriteSharedIndexData(ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_SHARED_INDEX_DATA);
	uint32 start = 0;
	uint32 count = 36;
	ser << start << count;
	outStream.EndChunk();
}

void WriteSubmeshInfo(ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_SUBMESH_INFO);
	uint8 primType = 2; // triangles
	ser << primType;
	outStream.EndChunk();
}

void WriteSubmeshData(ChunkOutputStream& outStream) {
	OutputSerializer& ser = outStream.BeginChunk(MCID_SUBMESH_DATA);
	WriteSubmeshInfo(outStream);
	WriteSharedVertexData(outStream);
	WriteSharedIndexData(outStream);
	outStream.BeginChunk(MCID_SUBMESH_END);
	outStream.EndChunk();
	outStream.EndChunk();
}

void TestDumpCubeMesh(nextar::ApplicationContext& application) {
	nextar::FileSystem& fileSys = nextar::FileSystem::Instance();

	String version("1.0.0");
	URL url("{EngineData}/Meshes/Box.nexmesh");
	OutputStreamPtr output = fileSys.OpenWrite(url, false);
	{
	OutputSerializer ser(output);
	uint32 magic = FILE_MAGIC_MESH;
	// output magic and version
	ser << magic << version;
	}
	ChunkOutputStream outStream(output);
	// MCID_MESH_HEADER: mesh header
	WriteMeshHeader(outStream);

	// MCID_VERTEX_DATA: shared vertex data
	WriteVertexData(outStream);

	// MCID_INDEX_DATA
	WriteIndexData(outStream);

	// MCID_BOUNDS_INFO
	WriteBoundsInfo(outStream);

	// MCID_MATERIAL_DATA
	WriteMaterialData(outStream);

	// MCID_SUBMESH_DATA
	WriteSubmeshData(outStream);

	outStream.BeginChunk(MCID_END);
	outStream.EndChunk();
}

int NextarMain(int argc, char* argv[]) {
	nextar::ACBaseImpl impl;
	nextar::ApplicationContext application("FileSystem", impl);
	application.InitializeContext(argc, argv);
	TestDumpCubeMesh(application);
	application.DestroyContext();
	std::cin.get();
	return 0;
}

