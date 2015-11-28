/**
 * Buggy.cpp
 * Created on: Thu Sep 17 21:09:03 IST 2015
 * @author: obhi
 */
 #include <Buggy.h>

 NEX_IMPLEMENT_APP()
 ;

 using namespace nextar;

void Buggy::_SetupScene(SceneAssetPtr& scene) {
  UTApplication::_SetupScene(scene);
  AxisAlignedBox box = AxisAlignedBox(-10, -10, -10, 10, 10, 10);
 // DebugDisplay::Instance().Register(box, Color::Blue);
  Matrix4x4 m = Matrix4x4::IdentityMatrix;
  //DebugDisplay::Instance().Register(m, 2);
  cameraEntity = scene->FindComponent(NamedObject::AsyncStringID("MainCamera"));
  if (cameraEntity) {
    Entity* e = static_cast<Entity*>(cameraEntity);
    // create a behaviour
    String deviceId;
    for(uint32 i = 0; i < InputManager::Instance().GetNumController(); ++i) {
      auto& desc = InputManager::Instance().GetControllerDesc(i);
      if (desc.type == ControllerType::TYPE_KEYBOARD_AND_MOUSE) {
        InputManager::Instance().RegisterController(desc.deviceId);
        deviceId = Convert::ToString(desc.deviceId);
        break;
      }
    }
    Behaviour* b = static_cast<Behaviour*>(Component::Instance(
      Component::CLASS_FPS_BEHAVIOR,
      NamedObject::AsyncStringID("FPSBehaviour")));
    Moveable* mv = Component::Instance<Moveable>(NamedObject::AsyncStringID("CameraTransform"));
    Quaternion rotation = QuatFromAxisAng(Vector3::XAxis, Math::PI_BY_4);
    mv->SetTransform(Vec3ASet(10, 50, -50), rotation, 1);
    e->AttachComponent(mv);
    e->AttachComponent(b);
    b->SetParamValue("device_id", deviceId);
  }

  Entity::Factory* entityFactory =
      static_cast<Entity::Factory*>(ComponentFactoryArchive::Instance().AsyncFindFactory(
          Entity::CLASS_ID));

  // load buggy and render deferred
  URL locator = URL("{EngineData}/Meshes/Box.mesh");
  MeshAssetPtr mesh =
      MeshAsset::Traits::Instance(NamedObject::AsyncStringID("Box"),
          locator);
  mesh->RequestLoad();
  EntityPtr meshEnt = entityFactory->AsyncCreateMeshEntity(
      NamedObject::AsyncStringID("Box"), mesh);
  meshEnt->SetTransform(Vec3ASet(0,0,0), QuatIdentity(), 1.0f);
  meshEnt->Update(this->frameTimer);
  meshEnt->AddToScene(scene);
  meshEntity = meshEnt;
}

void Buggy::Execute(const FrameTimer& frameTimer) {
  cameraEntity->Update(frameTimer);
  meshEntity->Update(frameTimer);
  UTApplication::Execute(frameTimer);
}

int NextarMain(int argc, char* argv[]) {
	Buggy application;
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
