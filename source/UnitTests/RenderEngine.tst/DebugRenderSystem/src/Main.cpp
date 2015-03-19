#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationMesh: public UTApplication {
public:
	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);
		AABox3 box;
		box.min.Set(-10, -10, -10);
		box.max.Set( 10,  10,  10);
		//DebugDisplay::Instance().Register(box, Color::Blue);
		Matrix4x4 m = Matrix4x4::IdentityMatrix;
		m = Mat4x4Scale(4, m);
		DebugDisplay::Instance().Register(m, Color::White);
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationMesh application;
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
