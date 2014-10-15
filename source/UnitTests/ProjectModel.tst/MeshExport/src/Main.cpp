#include <ProjectModelHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationMeshExport: public UTApplication {
public:
	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);
		MeshTemplate::ID id(NamedObject::AsyncStringID("Buggy"));
		URL location(FileSystem::ArchiveEngineData_Name, "Meshes/FBX/Buggy/buggy.FBX");
		
		MeshTemplatePtr mesh =
				MeshTemplate::Traits::Instance(id,
						location);
		mesh->RequestLoad();
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationMeshExport application;
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
