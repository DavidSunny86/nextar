#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationMesh: public UTApplication {
public:
	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);

		URL url("{EngineData}/Meshes/Box.nexmesh");
		MeshAsset::ID id(NEX_CSTR_ID("Box"));
		
		MeshAssetPtr mesh = MeshAsset::Traits::Instance(id, url);
		if (mesh) {
			mesh->RequestLoad();
		}

		Entity::Factory* entityFactory =
				static_cast<Entity::Factory*>(ComponentFactoryArchive::Instance().AsyncFindFactory(
						Entity::CLASS_ID));

		EntityPtr meshEnt = entityFactory->AsyncCreateMeshEntity(
				NEX_CSTR_ID("BoxEntity"), mesh);
		EntityPtr lightEnt = entityFactory->AsyncCreateOmniLightEntity(
				NEX_CSTR_ID("OmniLightEntity"), Color::White, 100);
		lightEnt->SetTransform(Vec3ASet(10,100,10), QuatIdentity(), 1);
		meshEnt->SetTransform(Vec3ASet(0,0,0), QuatIdentity(), 20);
		lightEnt->Update(this->frameTimer);
		meshEnt->Update(this->frameTimer);
		lightEnt->AddToScene(scene);
		meshEnt->AddToScene(scene);
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationMesh impl;
	nextar::ApplicationContext application("MeshTest", impl);
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
