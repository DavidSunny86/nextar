/**
 * ScreenQuad.cpp
 * Created on: Sun, Nov  1, 2015  1:36:52 AM
 * @author: obhi
*/

#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationImageLoader : public UTApplication {
	
public:

	void DisplayImage(const String& path) {
		URL texturePath(path);
		TextureAsset::ID id(StringUtils::GetStringID(texturePath.GetComputedName()));
		TextureAssetPtr texture = TextureAsset::Traits::Instance(id, texturePath);
		Box2D box1(0, 0, 1, 1);
		texture->RequestLoad();
		DebugDisplay::Register(box1, Color::White, Vec4ASet(0, 0, 1, 1), texture);
	}

	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);
		DisplayImage("{EngineData}/Textures/Default.png");
		
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationImageLoader impl;
	nextar::ApplicationContext application("ScreenQuad", impl);
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
