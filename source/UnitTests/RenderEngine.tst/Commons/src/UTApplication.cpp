#include <EngineHeaders.h>
#include <UTApplication.h>

UTApplication::UTApplication() :
		ApplicationContext("UTApplication") {
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

	EntityPtr camera = entityManager->AsyncCreateCameraEntity(
			NamedObject::AsyncStringID("Main"));
	camera->AddToScene(scene);
	window->CreateViewport(static_cast<Camera*>(camera->GetSpatial()));
}

void UTApplication::ConfigureExtendedInterfacesImpl() {
	_SetupRenderDriver();
	SceneAssetPtr scene = _CreateDefaultScene();
	_SetupScene(scene);
	RegisterListener(Listener(this, 0));
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
	window = *driver->AsyncGetContext(0)->GetRenderTargetList().begin();
}
