#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationMesh: public UTApplication {
public:
	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);

		MeshAsset::Factory* factory =
				static_cast<MeshAsset::Factory*>(ComponentFactoryArchive::Instance().AsyncFindFactory(
						MeshAsset::CLASS_ID));
		MeshAsset::Group* group =
				ComponentGroupArchive::Instance().AsyncFindOrCreate(
						StringUtils::DefaultID);
		URL url("{EngineData}/Meshes/Box.mesh");
		MeshAssetPtr mesh = MeshAsset::Traits::Instance(NamedObject::AsyncStringID("Box"),
				url, factory, group);
		if (mesh) {
			mesh->Load(false);
		}

		Entity::Factory* entityFactory =
				static_cast<Entity::Factory*>(ComponentFactoryArchive::Instance().AsyncFindFactory(
						Entity::CLASS_ID));

		EntityPtr meshEnt = entityFactory->AsyncCreateMeshEntity(
				NamedObject::AsyncStringID("BoxEntity"), mesh);
		EntityPtr lightEnt = entityFactory->AsyncCreateOmniLightEntity(
				NamedObject::AsyncStringID("OmniLightEntity"), Color::White, 100);
		lightEnt->SetTransform(Vec3ASet(10,100,10), QuatIdentity(), 1);
		meshEnt->SetTransform(Vec3ASet(0,0,0), QuatIdentity(), 20);
		lightEnt->Update();
		meshEnt->Update();
		lightEnt->AddToScene(scene);
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
