#include <EngineHeaders.h>
#include <UTApplication.h>

UTApplication::UTApplication() :
		EngineApplicationContext("UTApplication") {
}

UTApplication::~UTApplication() {
}

SceneAssetPtr UTApplication::_CreateDefaultScene() {
	return SceneAsset::Traits::Instance(
			NamedObject::AsyncStringID("UTScene"),
			StringUtils::DefaultID,
			StringUtils::DefaultID
			);
}

void UTApplication::_SetupScene(SceneAssetPtr& scene) {
	Entity::Factory* entityManager =
			static_cast<Entity::Factory*>(ComponentFactoryArchive::Instance().AsyncFindFactory(
					Entity::CLASS_ID));
	CullingSystem* culler = static_cast<CullingSystem*>(
		entityManager->AsyncCreate(Component::CLASS_BV_CULLER, NamedObject::AsyncStringID("MainCuller")));
	scene->SetCullingSystem(culler);
	EntityPtr camera = entityManager->AsyncCreateCameraEntity(
			NamedObject::AsyncStringID("Main"));
	// A camera at 50,0,50 looking at 0,0,0
	Quaternion rotation = QuatFromAxisAng(Vector3::ZAxis, 0);
	camera->SetTransform(Vec3ASet(50, 0, 50), rotation, 1);
	Camera::PerspectiveParams params;

	Size s = window->GetDimensions();
	params.aspectRatio = (float)s.dx / (float)s.dy;
	params.fieldOfView = Math::ToRadians(50);
	Camera::Traits::Cast(camera.GetPtr())->SetPerspectiveParams(params);
	Camera::Traits::Cast(camera.GetPtr())->SetNearDistance(0.1f);
	Camera::Traits::Cast(camera.GetPtr())->SetFarDistance(500.1f);

	camera->AddToScene(scene);

	window->CreateViewport(Camera::Traits::Cast(camera->GetSpatial()));
}

void UTApplication::ConfigureExtendedInterfacesImpl() {
	EngineApplicationContext::ConfigureExtendedInterfacesImpl();
	_SetupRenderDriver();
	scene = _CreateDefaultScene();
	_SetupScene(scene);
	RegisterListener(Listener(this, 0));
}

void UTApplication::BeginFrame(uint32 frameNumber) {
	RenderManager::Instance().RenderFrame(frameNumber);
}

void UTApplication::EndFrame(uint32 elapsedTime) {
}

void UTApplication::ReleaseResourcesImpl() {
	scene.Clear();
	window.Clear();
	EngineApplicationContext::ReleaseResourcesImpl();
}

void UTApplication::_SetupRenderDriver() {
	RenderManager::DriverCreationParams dcp;
	dcp.gpuIndex = 0;
	dcp.createDefaultContext = true;
	dcp.defaultContextParams.deferredContext = false;
	dcp.defaultContextParams.createDefaultWindow = true;
	dcp.defaultContextParams.defaultWindowWidth = 800;
	dcp.defaultContextParams.defaultWindowHeight = 600;
	dcp.defaultContextParams.depthBits = 24;
	dcp.defaultContextParams.stencilBits = 8;
	dcp.defaultContextParams.fullScreen = false;
	dcp.defaultContextParams.monitorIndex = 0;
	dcp.defaultContextParams.multiSamples = 0;
	dcp.defaultContextParams.stereo = false;
	dcp.defaultContextParams.reqOpenGLVersionMajor = 4;
	dcp.defaultContextParams.reqOpenGLVersionMinor = 2;
	dcp.defaultContextParams.sharedContextIndex = -1;
	dcp.defaultContextParams.extraParams["IsMainWindow"] = "true";
	dcp.defaultContextParams.extraParams["ExitOnClose"] = "true";
	dcp.defaultContextParams.extraParams["WindowLeft"] = "0";
	dcp.defaultContextParams.extraParams["WindowTop"] = "0";
	dcp.defaultContextParams.extraParams["WindowTitle"] = "UTApplication";

	RenderDriverPtr driver = RenderManager::Instance().AsyncCreateDriver(dcp);
	window = *driver->AsyncGetContext(0)->GetRenderTargetList().begin();
}
