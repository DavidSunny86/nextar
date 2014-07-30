#include <EngineHeaders.h>
#include <UTApplication.h>

UTApplication::UTApplication() :
	ProjectApplicationContext("UTApplication") {
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

	EntityPtr camera = entityManager->AsyncCreateCameraEntity(
			NamedObject::AsyncStringID("Main"));
	camera->AddToScene(scene);
}

void UTApplication::ConfigureExtendedInterfacesImpl() {
	EngineApplicationContext::ConfigureExtendedInterfacesImpl();
	scene = _CreateDefaultScene();
	_SetupScene(scene);
	RegisterListener(Listener(this, 0));
}

void UTApplication::BeginFrame(uint32 frameNumber) {
	RenderManager::Instance().RenderFrame(frameNumber);
}

void UTApplication::EndFrame(uint32 elapsedTime) {
}

void UTApplication::ReleaseResources() {
	scene.Clear();
}
