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
	virtual void _SetupScene(SceneAssetPtr& scene) {

		UTApplication::_SetupScene(scene);
		// load textures and display
		URL tiledTexture("{EngineData}/Textures/tiled_art.jpg");
		TextureAsset::ID id(NamedObject::AsyncStringID("TiledArt"));
		TextureAssetPtr texture = TextureAsset::Traits::Instance(id, tiledTexture);
		if (texture) {
			texture->RequestLoad();
			Box2D box(0, 0, 0.25f, 0.25f);
			DebugDisplay::Instance().Register(box, Color::White, Vec4ASet(0,0,1,1), texture);
		}
		
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationImageLoader application;
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
