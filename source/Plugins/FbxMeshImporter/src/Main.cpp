
#include <FbxMeshImporter.h>
#include <FbxMeshImporterApp.h>

NEX_IMPLEMENT_PLATFORM_APP()
;


int NextarMain(int argc, char* argv[]) {
	FbxMeshImporterApp application;
	application.InitializeContext(argc, argv);
	application.DestroyContext();
	return 0;
}
