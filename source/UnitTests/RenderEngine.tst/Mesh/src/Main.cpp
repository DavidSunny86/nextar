
#include "BaseHeaders.h"
#include "UTApplication.h"

NEX_IMPLEMENT_APP();

using namespace nextar;

class UTApplicationMesh : public UTApplication {
public:
	virtual void _SetupScene(ScenePtr& scene) {
		UTApplication::_SetupScene(scene);

		MeshAsset::Factory* factory = ComponentFactoryArchive::Instance().AsyncFindFactory(MeshAsset::CLASS_ID);
		MeshAsset::Group* group = ComponentGroupArchive::Instance().AsyncFindOrCreate(StringUtils::DefaultID);
		URL url("{EngineData}/mesh/box.mesh");
		MeshAssetPtr mesh = MeshAsset::Instance(factory, NamedObject::AsyncStringID("Box"), url, group);
		if (mesh) {
			mesh->Load(false);
		}

		Entity::Factory* entityFactory =
				static_cast<Entity::Factory*>(
						ComponentFactoryArchive::Instance().AsyncFindFactory(Entity::CLASS_ID));

		EntityPtr meshEnt = entityFactory->AsyncCreateMeshEntity(NamedObject::AsyncStringID("BoxEntity"), mesh);
		meshEnt->AddToScene(scene);
	}
};


int NextarMain(int argc, char* argv[]) {
	UTApplicationMesh application;
	application.InitializeContext();
	application.Run();
	application.DestroyContext();
	return 0;
}
