
#include "NexHeaders.h"
#include "UTApplication.h"

NEX_IMPLEMENT_APP();

using namespace nextar;

class UTApplicationMesh : public UTApplication {
public:
	virtual void _SetupScene() {
		MeshAssetManager* meshManager =
				static_cast<MeshAssetManager*>(
						ComponentManagerArchive::Instance().AsyncFindManager(MeshAsset::TYPE));
		MeshAssetPtr mesh = meshManager->AsyncCreateInstance("Mesh", "UTScene", URL("${EngineData}/mesh/box.mesh"));
		if (mesh) {
			mesh->Load(false);
		}
		EntityManager* entityManager =
				static_cast<EntityManager*>(
				ComponentManagerArchive::Instance().AsyncFindManager(Entity::TYPE));

		entityManager->AsyncCreateMeshEntity()
	}
};


int NextarMain(int argc, char* argv[]) {
	UTApplicationMesh application;
	application.InitializeContext();
	application.Run();
	application.DestroyContext();
	return 0;
}
