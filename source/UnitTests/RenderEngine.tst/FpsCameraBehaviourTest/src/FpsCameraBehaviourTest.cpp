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
		AxisAlignedBox box = AxisAlignedBox(-10, -10, -10, 10, 10, 10);
		DebugDisplay::Register(box, Color::Blue);
		Matrix4x4 m = Matrix4x4::IdentityMatrix;
		m = Mat4x4Scale(4, m);
		DebugDisplay::Register(m);
		entity = scene->FindComponent(StringUtils::GetStringID("MainCamera"));
		if (entity) {
			Entity* e = static_cast<Entity*>(entity);
			// create a behaviour
			String deviceId;
			for(uint32 i = 0; i < InputManager::Instance().GetNumController(); ++i) {
				auto& desc = InputManager::Instance().GetControllerDesc(i);
				if (desc.type == ControllerType::TYPE_XBOX360_CONTROLLER) {
					InputManager::Instance().RegisterController(desc.deviceId);
					deviceId = Convert::ToString(desc.deviceId);
					break;
				}
			}
			if (deviceId.length() == 0) {
				for (uint32 i = 0; i < InputManager::Instance().GetNumController(); ++i) {
					auto& desc = InputManager::Instance().GetControllerDesc(i);
					if (desc.type == ControllerType::TYPE_KEYBOARD_AND_MOUSE) {
						InputManager::Instance().RegisterController(desc.deviceId);
						deviceId = Convert::ToString(desc.deviceId);
						break;
					}
				}
			}
			Behaviour* b = static_cast<Behaviour*>(Component::Instance(
				Component::CLASS_FPS_BEHAVIOR,
				StringUtils::GetStringID("FPSBehaviour")));
			Moveable* mv = Component::Instance<Moveable>(StringUtils::GetStringID("CameraTransform"));
			Quaternion rotation = QuatFromAxisAng(Vector3::XAxis, Math::PI_BY_4);
			mv->SetTransform(Vec3ASet(0, 50, -50), rotation, 1);
			e->AttachComponent(mv);
			e->AttachComponent(b);
			b->SetParamValue("device_id", deviceId);
		}
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationFPSCamera impl;
	nextar::ApplicationContext application("FpsCameraTest", impl);
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
