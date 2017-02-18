#include <EngineHeaders.h>
#include <UTApplication.h>

UTApplication::UTApplication() :
	windowDimensions(800, 600) {
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
			StringUtils::GetStringID("UTScene"),
			StringUtils::DefaultID,
			StringUtils::DefaultID
			);
}

void UTApplication::_SetupScene(SceneAssetPtr& scene) {
	Entity::Factory* entityManager =
			static_cast<Entity::Factory*>(ComponentFactoryArchive::Instance().AsyncFindFactory(
					Entity::CLASS_ID));

	EntityPtr camera = entityManager->AsyncCreateCameraEntity(
			StringUtils::GetStringID("Main"));
	camera->AddToScene(scene);
}

void UTApplication::ConfigureExtendedInterfacesImpl(Config& config) {
	ProjectApplicationContext::ConfigureExtendedInterfacesImpl(config);
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
	ProjectApplicationContext::ReleaseResources();
	RenderManager::Instance().Destroy();
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

