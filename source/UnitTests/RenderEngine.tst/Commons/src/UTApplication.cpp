#include <EngineHeaders.h>
#include <UTApplication.h>

UTApplication::UTApplication() :
	EngineApplicationContext(), windowDimensions(800, 600) {
}

UTApplication::~UTApplication() {
}

void UTApplication::SetupScene(void* data) {
	reinterpret_cast<UTApplication*>(data)->SetupScene();
}

void UTApplication::SetupScene() {
	scene = _CreateDefaultScene();
	_SetupScene(scene);
}

SceneAssetPtr UTApplication::_CreateDefaultScene() {
	return SceneAsset::Traits::Instance(
			NEX_CSTR_ID("UTScene"),
			StringUtils::DefaultID,
			StringUtils::DefaultID
			);
}

void UTApplication::_SetupScene(SceneAssetPtr& scene) {
	Entity::Factory* entityManager =
			static_cast<Entity::Factory*>(ComponentFactoryArchive::Instance().AsyncFindFactory(
					Entity::CLASS_ID));
	CullingSystem* culler = static_cast<CullingSystem*>(
		entityManager->AsyncCreate(Component::CLASS_BV_CULLER, NEX_CSTR_ID("MainCuller")));
	scene->SetCullingSystem(culler);
	EntityPtr camera = entityManager->AsyncCreateCameraEntity(
			NEX_CSTR_ID("MainCamera"));
	// A camera at 50,0,50 looking at 0,0,0
	Quat::type rotation = QuatFromAxisAng(Vec3::type::XAxis, Math::PI_BY_4);
	camera->SetTransform(Vec3ASet(0, 50,-50), rotation, 1);
	Camera::PerspectiveParams params;

	Size s = window->GetDimensions();
	params.aspectRatio = (float)s.dx / (float)s.dy;
	params.fieldOfView = Math::ToRadians(50);
	Camera::Traits::Cast(camera.GetPtr())->SetPerspectiveParams(params);
	Camera::Traits::Cast(camera.GetPtr())->SetNearDistance(0.1f);
	Camera::Traits::Cast(camera.GetPtr())->SetFarDistance(500.1f);

	camera->AddToScene(scene);

	window->CreateViewport(Camera::Traits::Cast(camera->GetSpatial()), GetRenderSystemsToInstantiate())->SetClearColor(Color(1, 1, 1, 1));
}

void UTApplication::ConfigureExtendedInterfacesImpl(Config& config) {
	EngineApplicationContext::ConfigureExtendedInterfacesImpl(config);
	if (!RenderManager::QueryService(config))
		NEX_THROW_FatalError(EXCEPT_ASSERTION_FAILED);
	_SetupRenderDriver();
	ApplicationContext::Instance().RegisterListener(ApplicationContext::Listener(this, 0));
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_INIT_RESOURCES, SetupScene, this);
}

void UTApplication::Execute(const FrameTimer& frameTimer) {
	RenderManager::Instance().RenderFrame(frameTimer);
}

void UTApplication::ReleaseResources() {
	scene.Clear();
	window.Clear();
	EngineApplicationContext::ReleaseResources();
	RenderManager::Instance().Destroy();
}

String UTApplication::GetRenderSystemsToInstantiate() const {
	return "Default";
}

void UTApplication::_SetupRenderDriver() {
	RenderManager::DriverCreationParams dcp;
	dcp.gpuIndex = 0;
	dcp.createDefaultContext = true;
	dcp.defaultContextParams.deferredContext = false;
	dcp.defaultContextParams.createDefaultWindow = true;
	dcp.defaultContextParams.defaultWindowWidth = windowDimensions.dx;
	dcp.defaultContextParams.defaultWindowHeight = windowDimensions.dy;
	dcp.defaultContextParams.depthBits = 24;
	dcp.defaultContextParams.stencilBits = 8;
	dcp.defaultContextParams.fullScreen = false;
	dcp.defaultContextParams.monitorIndex = 0;
	dcp.defaultContextParams.multiSamples = 0;
	dcp.defaultContextParams.stereo = false;
	dcp.defaultContextParams.reqOpenGLVersionMajor = 4;
	dcp.defaultContextParams.reqOpenGLVersionMinor = 1;
	dcp.defaultContextParams.sharedContextIndex = -1;
	dcp.defaultContextParams.extraParams["IsMainWindow"] = "true";
	dcp.defaultContextParams.extraParams["ExitOnClose"] = "true";
	dcp.defaultContextParams.extraParams["WindowLeft"] = "0";
	dcp.defaultContextParams.extraParams["WindowTop"] = "0";
	dcp.defaultContextParams.extraParams["WindowTitle"] = "UTApplication";
	RenderDriverPtr driver = RenderManager::Instance().AsyncCreateDriver(dcp);
	window = *driver->AsyncGetContext(0)->GetRenderTargetList().begin();
}
