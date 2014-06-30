/*
 * Main.cpp
 *
 *  Created on: 04-Mar-2014
 *      Author: obhi
 */
#include <BaseHeaders.h>

NEX_IMPLEMENT_PLATFORM_APP()
;

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

enum MeshFileMagic {
	FILE_MAGIC_MESH = 0xff811901
};

using namespace nextar;

void TestDumpCubeMesh(nextar::ApplicationContext& application) {
	nextar::FileSystem& fileSys = nextar::FileSystem::Instance();

	String version("1.0.0");
	URL url("{EngineData}/Meshes/box.mesh");
	OutputStreamPtr output = fileSys.OpenWrite(url, false);
	OutputSerializer ser(output);

	uint32 magic = FILE_MAGIC_MESH;
	// output magic and version
	ser << magic << version;

	// MCID_MESH_HEADER: mesh header
	OutputSerializer::ChunkHeader header;
	header.first = MCID_MESH_HEADER;
	header.second = 4;
	magic = 1;
	ser << header << magic;

	//

}

int NextarMain(int argc, char* argv[]) {
	nextar::ApplicationContext application("FileSystem");
	application.InitializeContext();
	TestDumpCubeMesh(application);
	application.DestroyContext();
	std::cin.get();
	return 0;
}

