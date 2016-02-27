#include <BaseHeaders.h>
#include <ApplicationContext.h>
#include <PluginRegistry.h>
#include <PropertyInterface.h>
#include <WindowManager.h>
#include <BackgroundStreamer.h>
#include <BackgroundStreamerImpl.h>
#include <NamedObject.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(ApplicationContext);

String ApplicationContext::_configPathName = "${EngineData}/Configs/Config.cfg";

ApplicationContext::ApplicationContext(const String& name) :
		appName(name), quitting(false), runningLoop(false), errorHandler(nullptr),
		resourcesDestroyed(false) {
	NEX_NEW(LogManager());
	// initialize the pools
#ifdef NEX_DEBUG
	// add a default logger
	static Logger logger("Debug");
	LogManager::Instance().AddNewLogger(&logger, true);
#endif
}

ApplicationContext::~ApplicationContext(void) {
	NEX_DELETE(LogManager::InstancePtr());
}

void ApplicationContext::ParseCommandLineParaqms(int argc, char* argv[]) {
	NameValueMap appParams;
	for (int i = 1; i < argc; ++i) {
		String arg = argv[i];
		if (arg[0] == '-') {
			String value;
			size_t pos = arg.find_first_of('=');
			if (pos == String::npos) {
				value = "true";
				pos = arg.length();
			} else {
				value = arg.substr(pos+1);
			}
			appParams[arg.substr(1, pos - 1)] = value;

		}
	}

	ParseCommandLine(appParams);
}

void ApplicationContext::InitializeContext(int argc, char* argv[]) {
	Trace("Initializing application context: " + appName);
	CreateServices();
	ParseCommandLineParaqms(argc, argv);
	LoadConfiguration();
	// load sub-plugins
	ConfigureServices();
	PluginRegistry::Instance().RequestPlugins(PLUG_TYPE_LIFETIME,
			StringUtils::Null, true);
	ConfigureExtendedInterfacesImpl();
	DispatchEvent(EVENT_INIT_RESOURCES);
	// in case it was loaded
	NamedObject::OnFlushStrings();
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
}

void ApplicationContext::ReleaseResources() {
	if (resourcesDestroyed)
		return;
	DispatchEvent(EVENT_DESTROY_RESOURCES);
	ReleaseResourcesImpl();
	resourcesDestroyed = true;
}

void ApplicationContext::DestroyContext() {
	Trace("Destroying application context: " + appName);
	SaveConfiguration();
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
	// close services
	DestroyExtendedInterfacesImpl();
	// todo Moved: RenderPass
	// NEX_ASSERT (!RenderManager::InstancePtr());
	// todo Moved: Engine
	// NEX_DELETE(ComponentFactoryArchive::InstancePtr());
	NEX_DELETE(BackgroundStreamer::InstancePtr());
	NEX_DELETE(WindowManager::InstancePtr());

	// unload plugins
	PluginRegistry::Instance().RequestPlugins(PLUG_TYPE_LIFETIME,
			StringUtils::Null, false);

	NEX_DELETE(PluginRegistry::InstancePtr());
	// just save the strings
	NamedObject::OnExit();
	// destroy dictionaries
	PropertyInterface::DestroyDictionaries();
	NEX_DELETE(FileSystem::InstancePtr());
}

void ApplicationContext::SetQuitting(bool value) {
	quitting = value;
}

void ApplicationContext::QuitApplication() {
	if (!quitting) {
		quitting = true;
		WindowManager::Instance().Quit();
	}
}

void ApplicationContext::Run() {

	frameClock.StartClock();

	while (!quitting) {
		
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
			for (auto& p : frameListeners)
				p.frameListener->Execute(frameTimer);

			frameTimer.EndFrame(frameClock.Tick());
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
	ReleaseResources();
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

void ApplicationContext::ShowErrorDialog(int errorCode, const String& errorText) {
	if (errorHandler) {
		errorHandler->ShowErrorDialog(errorCode, errorText);
	}
}

}
