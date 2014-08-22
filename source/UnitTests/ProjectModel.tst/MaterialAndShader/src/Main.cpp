#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationMesh: public UTApplication {
public:
	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);
		MaterialTemplatePtr material;
		ShaderTemplatePtr shader;

		MaterialTemplate::ID id(NamedObject::AsyncStringID("Box"));
		URL loadUrl("{EngineData}/Scripts/Material/DefaultMaterials.mtl");
		URL saveUrl("{EngineData}/Materials/Box.mtl");
		URL saveShaderUrl("{EngineData}/Shaders/Deferred.nfx");
		material = MaterialTemplate::Traits::Instance(id, loadUrl);
		material->Load(false);
		material->SetAssetLocator(saveUrl);
		shader = material->GetShaderTemplate();
		shader->SetAssetLocator(saveShaderUrl);
		shader->Save(false);
		material->Save(false);

		// save engine lights
		
		MaterialTemplate::ID id2(NamedObject::AsyncStringID("DeferredLights"));
		URL dlMaterialUrl("{EngineData}/Scripts/Material/DeferredLights.mtl");
		URL saveDlMaterialUrl("{EngineData}/Materials/DeferredLights.mtl");
		URL saveDlShaderUrl("{EngineData}/Shaders/DeferredLights.nfx");
		material = MaterialTemplate::Traits::Instance(id2, dlMaterialUrl);
		material->Load(false);
		shader = material->GetShaderTemplate();
		shader->SetAssetLocator(saveDlShaderUrl);
		shader->Save(false);
		AssetPtr mtl = material;
		Asset::AsyncSave(mtl, saveDlMaterialUrl);
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationMesh application;
	application.InitializeContext();
	application.Run();
	application.DestroyContext();
	return 0;
}
