
#include <BaseHeaders.h>
#include <UTApplication.h>


UTApplication::UTApplication() : ApplicationContext("UTApplication") {
}

ScenePtr UTApplication::_CreateDefaultScene() {
	SceneManager* sceneManager =
			static_cast<SceneManager*>(ComponentFactoryArchive::Instance().AsyncFindManager(
			Scene::CLASS_ID));
	return sceneManager->AsyncCreateEmptyScene("UTScene");
}

void UTApplication::_SetupScene(ScenePtr& scene) {
	EntityManager* entityManager =
			static_cast<EntityManager*>(
			ComponentFactoryArchive::Instance().AsyncFindManager(Entity::CLASS_ID));

	EntityPtr camera = entityManager->AsyncCreateCameraEntity("Main");
	camera->AddToScene(scene);
	window->CreateViewport(camera->GetCamera());
}

void UTApplication::ConfigureExtendedInterfacesImpl() {
	_SetupRenderDriver();
	ScenePtr scene = _CreateDefaultScene();
	_SetupScene(scene);
	AsyncRegisterListener(Listener(this, 0));
}

void UTApplication::BeginFrame(uint32 frameNumber) {
	RenderManager::Instance().RenderFrame(frameNumber);
}

void UTApplication::EndFrame(uint32 elapsedTime) {
}

void UTApplication::_SetupRenderDriver() {
	RenderManager::DriverCreationParams dcp;
	dcp.gpuIndex = 0;
	dcp.createDefaultContext = true;
	dcp.defaultContextParams.createDefaultWindow = true;
	dcp.defaultContextParams.defaultWindowWidth = 800;
	dcp.defaultContextParams.defaultWindowHeight = 600;
	dcp.defaultContextParams.depthBits = 24;
	dcp.defaultContextParams.stencilBits = 8;
	dcp.defaultContextParams.fullScreen = false;
	dcp.defaultContextParams.monitorIndex = 0;
	dcp.defaultContextParams.multiSamples = 0;
	dcp.defaultContextParams.reqOpenGLVersionMajor = 3;
	dcp.defaultContextParams.reqOpenGLVersionMinor = 2;
	dcp.defaultContextParams.sharedContextIndex = -1;
	dcp.defaultContextParams.extraParams["IsMainWindow"] = "true";
	dcp.defaultContextParams.extraParams["ExitOnClose"] = "true";
	dcp.defaultContextParams.extraParams["WindowLeft"] = "0";
	dcp.defaultContextParams.extraParams["WindowTop"] = "0";
	dcp.defaultContextParams.extraParams["WindowTitle"] = "UTApplication";

	RenderDriverPtr driver = RenderManager::Instance().AsyncCreateDriver(dcp);
	window = driver->AsyncGetContext(0)->GetRenderWindow(0);
}
