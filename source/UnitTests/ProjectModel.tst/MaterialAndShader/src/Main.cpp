#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationMesh: public UTApplication {
public:
	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);

		MaterialTemplate::ID id(NamedObject::AsyncStringID("Box"));
		URL loadUrl("{EngineData}/Scripts/Material/DefaultMaterials.mtl");
		URL saveUrl("{EngineData}/Materials/Box.mtl");
		URL saveShaderUrl("{EngineData}/Shaders/Deferred.nfx");
		MaterialTemplatePtr material = MaterialTemplate::Traits::Instance(id, loadUrl);
		material->Load(false);
		material->SetAssetLocator(saveUrl);
		ShaderTemplatePtr shader = material->GetShaderTemplate();
		shader->SetAssetLocator(saveShaderUrl);
		shader->Save(false);
		material->Save(false);
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationMesh application;
	application.InitializeContext();
	application.Run();
	application.DestroyContext();
	return 0;
}
