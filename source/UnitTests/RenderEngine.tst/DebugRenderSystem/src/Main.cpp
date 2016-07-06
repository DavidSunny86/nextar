#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationMesh: public UTApplication {
public:
	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);
		AxisAlignedBox box =  AxisAlignedBox(-10, -10, -10, 10,  10,  10);
		//DebugDisplay::Register(box, Color::Blue);
		Matrix4x4 m = Matrix4x4::IdentityMatrix;
		m = Mat4x4Scale(4, m);
		DebugDisplay::Register(m);
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationMesh impl;
	nextar::ApplicationContext application("DebugRenderSystem", impl);
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
