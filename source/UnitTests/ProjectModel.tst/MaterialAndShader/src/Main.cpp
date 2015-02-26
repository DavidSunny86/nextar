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
		URL loadUrl("{EngineData}/Scripts/Materials/DefaultMaterials.mtl");
		URL saveUrl("{EngineData}/Materials/Box.mtl");
		URL saveShaderUrl("{EngineData}/Shaders/Deferred.nfx");
		material = MaterialTemplate::Traits::Instance(id, loadUrl);
		material->RequestLoad();
		material->SetAssetLocator(saveUrl);
		shader = material->GetShaderTemplate();
		shader->SetAssetLocator(saveShaderUrl);
		shader->RequestSave();
		material->RequestSave();

		// save engine lights	
		MaterialTemplate::ID id2(NamedObject::AsyncStringID("DeferredLights"));
		URL dlMaterialUrl("{EngineData}/Scripts/Materials/DeferredLights.mtl");
		URL saveDlMaterialUrl("{EngineData}/Materials/DeferredLights.mtl");
		URL saveDlShaderUrl("{EngineData}/Shaders/DeferredLights.nfx");
		material = MaterialTemplate::Traits::Instance(id2, dlMaterialUrl);
		material->RequestLoad();
		shader = material->GetShaderTemplate();
		shader->SetAssetLocator(saveDlShaderUrl);
		shader->RequestSave();
		AssetPtr mtl = material;
		Asset::AssetSave(mtl, saveDlMaterialUrl);

		MaterialTemplate::ID id3(NamedObject::AsyncStringID("Debug3D"));
		URL dbMaterialUrl("{EngineData}/Scripts/Materials/Debug3D.mtl");
		URL saveDbMaterialUrl("{EngineData}/Materials/Debug3D.mtl");
		URL saveDbShaderUrl("{EngineData}/Shaders/Debug3D.nfx");
		material = MaterialTemplate::Traits::Instance(id3, dbMaterialUrl);
		material->RequestLoad();
		shader = material->GetShaderTemplate();
		shader->SetAssetLocator(saveDbShaderUrl);
		shader->RequestSave();
		mtl = material;
		Asset::AssetSave(mtl, saveDbMaterialUrl);
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationMesh application;
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
