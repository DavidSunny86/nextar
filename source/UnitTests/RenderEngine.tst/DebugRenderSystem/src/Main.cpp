#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationMesh: public UTApplication {
public:
	virtual String GetRenderSystemsToInstantiate() const override {
		return "Debug";
	}

	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);
		AxisAlignedBox box =  AxisAlignedBox(-10, -10, -10, 10,  10,  10);
		DebugDisplay::Register(box, Color::Blue);
		Mat4::type m = Mat4::type::IdentityMatrix;
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
