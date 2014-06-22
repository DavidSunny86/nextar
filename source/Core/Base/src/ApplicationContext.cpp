#include <BaseHeaders.h>
#include <ApplicationContext.h>
#include <PluginRegistry.h>
#include <PropertyInterface.h>
#include <WindowManager.h>
#include <BackgroundStreamer.h>
#include <BackgroundStreamerImpl.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(ApplicationContext);

String ApplicationContext::_configPathName = "${Bin}/Config.cfg";

ApplicationContext::ApplicationContext(const String& name) :
		appName(name), quitting(false), runningLoop(false) {
	NEX_NEW(LogManager());
#ifdef NEX_DEBUG
	// add a default logger
	static Logger logger("Debug");
	LogManager::Instance().AddNewLogger(&logger, true);
#endif
}

ApplicationContext::~ApplicationContext(void) {
	NEX_DELETE(LogManager::InstancePtr());
}

void ApplicationContext::InitializeContext() {
	Trace("Initializing application context: " + appName);
	CreateServices();
	LoadConfiguration();
	// load sub-plugins
	PluginRegistry::Instance().RequestPlugins(PLUG_TYPE_LIFETIME,
			StringUtils::Null, true);

	ConfigureServices();
	ConfigureExtendedInterfacesImpl();
}

void ApplicationContext::LoadConfiguration() {
	// this is absolutely necessary that we do it here
	// as file system is needed to resolve URLs
	defaultConfigPath = URL(_configPathName);
	try {
		config.LoadConfiguration(defaultConfigPath);
	} catch (GracefulErrorExcept& ex) {
		Error(ex.GetMsg());
	}
}

void ApplicationContext::ConfigureServices() {
	// ready filesystem
	FileSystem::Instance().Configure(config);
	// load plugins
	PluginRegistry::Instance().Configure(config);
	// todo Moved: RenderManager
	// if render manager is present, configure it
	// if (RenderManager::InstancePtr())
	//	RenderManager::Instance().Configure(config);
}

void ApplicationContext::CloseServices() {
	// todo Moved: RenderManager
	// if render manager is present, close it
	// if (RenderManager::InstancePtr())
	// 	RenderManager::Instance().Close();
}

void ApplicationContext::DestroyContext() {
	Trace("Destroying application context: " + appName);
	SaveConfiguration();
	// destroy managers
	// destroy dictionaries
	PropertyInterface::DestroyDictionaries();
	// close services
	CloseServices();
	// unload plugins
	PluginRegistry::Instance().RequestPlugins(PLUG_TYPE_LIFETIME,
			StringUtils::Null, false);
	// services
	DestroyServices();
}

void ApplicationContext::CreateServices() {
	NEX_NEW(FileSystem());
	NEX_NEW(PluginRegistry());
	NEX_NEW(WindowManager());
	NEX_NEW(BackgroundStreamerImpl());
	// todo Moved to Engine
	//NEX_NEW(ComponentFactoryArchive());

	CreateExtendedInterfacesImpl();

}

void ApplicationContext::DestroyServices() {

	DestroyExtendedInterfacesImpl();
	// todo Moved: RenderSystem
	// NEX_ASSERT (!RenderManager::InstancePtr());
	// todo Moved: Engine
	// NEX_DELETE(ComponentFactoryArchive::InstancePtr());
	NEX_DELETE(BackgroundStreamer::InstancePtr());
	NEX_DELETE(WindowManager::InstancePtr());
	NEX_DELETE(PluginRegistry::InstancePtr());
	NEX_DELETE(FileSystem::InstancePtr());
}

void ApplicationContext::SetQuitting(bool value) {
	quitting = value;
}

void ApplicationContext::Run() {

	frameClock.StartClock();

	while (!quitting) {
		frameClock.Tick();

		if (frameListenersToAdd.size()) {
			// process any add requests
			frameListeners.insert(frameListenersToAdd.begin(),
					frameListenersToAdd.end());
			frameListenersToAdd.clear();
		}
		{
			runningLoop = true;
			frameTimer.BeginFrame();
			WindowManager::Instance().ProcessMessages();
			// run the frame listeners
			FrameListenerSet::iterator it = frameListeners.begin();
			FrameListenerSet::iterator en = frameListeners.end();
			for (; it != en; ++it)
				(*it).frameListener->BeginFrame(frameTimer.GetFrameNumber());

			uint32 elapsedMilSec = frameClock.Tick();

			it = frameListeners.begin();
			for (; it != en; ++it)
				(*it).frameListener->EndFrame(elapsedMilSec);
			frameTimer.EndFrame(elapsedMilSec);
			runningLoop = false;
		}

		if (frameListenersToRemove.size()) {
			// process any removal requests
			FrameListenerSet::iterator it = frameListenersToRemove.begin();
			FrameListenerSet::iterator en = frameListenersToRemove.end();
			for (; it != en; ++it)
				frameListeners.erase(*it);
			frameListenersToRemove.clear();
		}
	}
	frameClock.StopClock();
}

void ApplicationContext::RegisterListener(const Listener& l) {
	if (runningLoop)
		frameListenersToAdd.insert(l);
	else
		frameListeners.insert(l);
}

void ApplicationContext::UnregisterListener(const Listener& l) {
	if (runningLoop)
		frameListenersToRemove.insert(l);
	else
		frameListeners.erase(l);
}

}
