
#include "NexHeaders.h"
#include "UTApplication.h"

NEX_IMPLEMENT_APP();

using namespace nextar;

class UTApplicationMesh : public UTApplication {
public:
	virtual void _SetupScene() {
		MeshPtr mesh = defaultMeshManager.AsyncCreateInstance("Mesh", "UTScene", URL("${EngineData}/mesh/box.mesh"));
		if (mesh) {
			mesh->Load(false);
		}
	}
};


int NextarMain(int argc, char* argv[]) {
	UTApplicationMesh application;
	application.InitializeContext();
	application.Run();
	application.DestroyContext();
	return 0;
}
