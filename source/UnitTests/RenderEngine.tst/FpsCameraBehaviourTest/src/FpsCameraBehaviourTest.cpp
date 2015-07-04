/**
 * FpsCameraBehaviourTest.cpp
 * Created on: Sat Jul  4 18:07:50 IST 2015
 * @author: obhi
 */
#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationFPSCamera: public UTApplication {
public:

	Component* entity;

	virtual void Execute(const FrameTimer& frameTimer) override {
		entity->Update(frameTimer);
		UTApplication::Execute(frameTimer);
	}

	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);
		AABox3 box;
		box.min.Set(-10, -10, -10);
		box.max.Set( 10,  10,  10);
		DebugDisplay::Instance().Register(box, Color::Blue);
		Matrix4x4 m = Matrix4x4::IdentityMatrix;
		m = Mat4x4Scale(4, m);
		DebugDisplay::Instance().Register(m, Color::White);
		entity = scene->FindComponent(NamedObject::AsyncStringID("MainCamera"));
		if (entity) {
			Entity* e = static_cast<Entity*>(entity);
			// create a behaviour
			String deviceId;
			for(uint32 i = 0; i < InputManager::Instance().GetNumController(); ++i) {
				auto& desc = InputManager::Instance().GetControllerDesc(i);
				if (desc.type == ControllerType::TYPE_XBOX360_CONTROLLER) {
					InputManager::Instance().RegisterController(desc.deviceId);
					deviceId = Convert::ToString(desc.deviceId);
				}
			}
			Behaviour* b = static_cast<Behaviour*>(Component::Instance(
					Component::CLASS_FPS_BEHAVIOR,
					NamedObject::AsyncStringID("FPSBehaviour")));
			Component* mv = Component::Instance(Moveable::CLASS_ID, NamedObject::AsyncStringID("CameraTransform"));
			e->AttachComponent(mv);
			e->AttachComponent(b);
			b->SetParamValue("DeviceID", deviceId);
		}
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationFPSCamera application;
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
