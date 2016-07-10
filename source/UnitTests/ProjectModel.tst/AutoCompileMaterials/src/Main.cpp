#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationAutoCompileMtl: public UTApplication,
	public Archive::ScanCallback {
	NEX_LOG_HELPER(UTApplicationAutoCompileMtl);
public:
	
	UTApplicationAutoCompileMtl() {
		windowDimensions.dx = 2;
		windowDimensions.dy = 2;
	}


	virtual void FoundFile(const FileAttribute& attribute, Archive* arch) {
		Trace("File: " + attribute.fileName.ToString());
		String materialName = attribute.fileName.GetComputedName();

		MaterialTemplate::ID id(NamedObject::AsyncStringID(materialName));
		URL materialSaveUrl("{EngineData}/Materials/" + materialName + ".mtl");
		URL materialAssetSaveUrl("{EngineData}/Materials/Assets/" + materialName + ".asset");
		MaterialTemplatePtr material = MaterialTemplate::Traits::Instance(id, attribute.fileName);
		material->RequestLoad();
		if (material->AsyncIsLoaded()) {
			ShaderTemplatePtr shader = material->GetShaderTemplate();
			if (shader) {
				URL locator = shader->GetAssetLocator();
				URL shaderSaveUrl("{EngineData}/Shaders/" + locator.GetComputedName() + ".nfx");
				shader->SetAssetLocator(shaderSaveUrl);
				material->SetAssetLocator(materialSaveUrl);
				shader->RequestSave();
				material->RequestSave();
				AssetPtr asset = material;
				Asset::AssetSave(asset, materialAssetSaveUrl, "MTL");
				return;
			}
		}
		Error("Failed to load: " + attribute.fileName.ToString());
	}

	void Execute() {
		ArchivePtr arch = FileSystem::Instance().OpenArchive("EngineData");
		if (arch) {
			arch->Scan(this, "Scripts/Materials/*.mscript");
		}
		ApplicationContext::Instance().QuitApplication();
	}

	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);		
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationAutoCompileMtl impl;
	nextar::ApplicationContext application("AutoCompileMaterials", impl);
	application.InitializeContext(argc, argv);
	impl.Execute();
	application.DestroyContext();
	return 0;
}
