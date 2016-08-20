#include <ProjectModelHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

/**
 * Command Line:
 * --file=Meshes/FBX/Box.FBX 
 * --save=Meshes/Box.mesh
*/
class UTApplicationMeshExport: public UTApplication {
	String filePath;
	String savePath;
public:
	UTApplicationMeshExport(const String& file, const String& save) : filePath(file), savePath(save) {}
	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);
		
		URL location(FileSystem::ArchiveEngineData_Name, filePath);
		MeshTemplate::ID id(NamedObject::AsyncStringID(location.GetComputedName()));
		
		MeshTemplatePtr mesh =
				MeshTemplate::Traits::Instance(id,
						location);
		mesh->RequestLoad();
		StreamInfo streamInfo;
		streamInfo.locator = URL(FileSystem::ArchiveEngineData_Name, savePath);
		mesh->RequestSave(streamInfo);
		ApplicationContext::Instance().QuitApplication();
	}
};

int NextarMain(int argc, char* argv[]) {
	
	String filePath, savePath;
	for (int i = 1; i < argc; ++i) {
		char* suffix = std::strstr(argv[i], "--file=");
		if (suffix) {
			suffix += sizeof("--file");
			filePath = suffix;
		} else {
			char* out = std::strstr(argv[i], "--save=");
			if (out) {
				out += sizeof("--save");
				savePath = out;
			}
		}
	}
	if (filePath.length()) {
		UTApplicationMeshExport impl(filePath, savePath.length() > 0 ? savePath : filePath + ".nexmesh");
		nextar::ApplicationContext application("MeshExport", impl);
		application.InitializeContext(argc, argv);
		application.Run();
		application.DestroyContext();
	}
	return 0;
}
