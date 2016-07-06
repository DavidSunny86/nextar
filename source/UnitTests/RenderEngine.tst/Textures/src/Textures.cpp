/**
 * Textures.cpp
 * Created on: Sat, Nov  7, 2015  4:43:57 PM
 * @author: obhi
 */

#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationTextureTests : public UTApplication {

public:

	void DisplayImage(const String& path, float x, float y, float tileX = 1, float tileY = 1) {
		URL texturePath(path);
		TextureAsset::ID id(NamedObject::AsyncStringID(texturePath.GetComputedName()));
		TextureAssetPtr texture = TextureAsset::Traits::Instance(id, texturePath);
		Box2D box1(x, y, x + 0.25f, y + 0.25f);
		texture->RequestLoad();
		DebugDisplay::Register(box1, Color::White, Vec4ASet(0, 0, tileX, tileY), texture);
	}
	virtual void _SetupScene(SceneAssetPtr& scene) {

		UTApplication::_SetupScene(scene);
		// load textures and display
		// test a png image
		DisplayImage("{EngineData}/Textures/DefaultTexture.png", 0, 0);
		// test a jpg load
		DisplayImage("{EngineData}/Textures/Water.jpg", 0.25f, 0);
		// test a tga load
		DisplayImage("{EngineData}/Textures/Angel.tga", 0.5f, 0);
		// test a dds load
		DisplayImage("{EngineData}/Textures/Bottle.dds", 0.75f, 0);
		// next batch
		DisplayImage("{EngineData}/Textures/Tree.dds", 0.0f, 0.25f);
		DisplayImage("{EngineData}/Textures/Globe.dds", 0.25f, 0.25f);
		DisplayImage("{EngineData}/Textures/Road.dds", 0.5f, 0.25f);
		DisplayImage("{EngineData}/Textures/Painting.dds", 0.75f, 0.25f);
		// next batch
		DisplayImage("{EngineData}/Textures/Landscape.dds", 0.0f, 0.5f);
		DisplayImage("{EngineData}/Textures/Books.dds", 0.25f, 0.5f);
		DisplayImage("{EngineData}/Textures/Resto.dds", 0.5f, 0.5f);
		DisplayImage("{EngineData}/Textures/Trom.dds", 0.75f, 0.5f);
		// next batch
		DisplayImage("{EngineData}/Textures/Girl.dds", 0.0f, 0.75f);
		DisplayImage("{EngineData}/Textures/Rouge.dds", 0.25f, 0.75f);
		// test tiling and repeat
		DisplayImage("{EngineData}/Textures/Lights.dds", 0.5f, 0.75f, 4, 4);
		DisplayImage("{EngineData}/Textures/Eiffel.dds", 0.75f, 0.75f);
		// test a compressed dds load
		// test a tga save
		// test coordinate system

	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationTextureTests impl;
	nextar::ApplicationContext application("Textures", impl);
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
