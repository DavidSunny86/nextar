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

ApplicationContext::ApplicationContext(const String& name, Impl& impl) :
quitting(false), errorHandler(nullptr),
_resourcesDestroyed(false), _pImpl(impl) {

	NEX_NEW(LogManager());
	SetAppName(name);
	SetProjectName(StringUtils::Default);
	SetProjectVersion(NEX_MAKE_VERSION(1, 0, 0));
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

void ApplicationContext::ParseCommandLineParams(int argc, char* argv[]) {
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
				value = arg.substr(pos + 1);
			}
			appParams[arg.substr(1, pos - 1)] = value;

		}
	}

	_pImpl.ParseCommandLine(appParams);
}

void ApplicationContext::InitializeContext(int argc, char* argv[]) {
	Trace("Initializing application context: " + _desc._name);
	ParseCommandLineParams(argc, argv);
	_pImpl.CreateServices();

	LoadConfiguration();
	// load sub-plugins
	_pImpl.ConfigureServices(config);

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
		_pImpl.LoadConfiguration();
	}
	catch (GracefulErrorExcept& ex) {
		Error(ex.GetMsg());
	}
}

void ApplicationContext::SaveConfiguration() {
	_pImpl.SaveConfiguration();
}

void ApplicationContext::DestroyContext() {
	Trace("Destroying application context: " + _desc._name);
	SaveConfiguration();
	// services
	_pImpl.DestroyServices();
}

void ApplicationContext::SetQuitting(bool value) {
	quitting = value;
}

void ApplicationContext::QuitApplication() {
	if (!quitting) {
		SetQuitting(true);
		_pImpl.QuitApplication();
	}
}

void ApplicationContext::Run() {

	frameClock.StartClock();

	while (!quitting && _pImpl.Step(frameClock));
	
	
	frameClock.StopClock();
	ReleaseResources();
}

void ApplicationContext::RegisterListener(const Listener& l) {
	_pImpl.RegisterListener(l);
}

void ApplicationContext::UnregisterListener(const Listener& l) {
	_pImpl.UnregisterListener(l);
}

void ApplicationContext::ShowErrorDialog(int errorCode, const String& errorText) {
	if (errorHandler) {
		errorHandler->ShowErrorDialog(errorCode, errorText);
	}
}

void ApplicationContext::ReleaseResources() {
	if (_resourcesDestroyed)
		return;
	DispatchEvent(EVENT_DESTROY_RESOURCES);
	_pImpl.ReleaseResources();
	_resourcesDestroyed = true;
}

/**************************************************************************
 *                          ACBaseImpl
 **************************************************************************/
ApplicationContextType ACBaseImpl::GetType() const {
	return ApplicationContextType(CONTEXT_BASE);
}

void ACBaseImpl::CreateServices() {
	NEX_NEW(FileSystem());
	NEX_NEW(PluginRegistry());
	NEX_NEW(WindowManager());
	NEX_NEW(BackgroundStreamerImpl());
	CreateExtendedInterfacesImpl();
}


void ACBaseImpl::ConfigureServices(Config& config) {
	// ready filesystem
	FileSystem::Instance().Configure(config);
	// load plugins
	PluginRegistry::Instance().Configure(config);

	PluginRegistry::Instance().RequestPlugins(PLUG_TYPE_LIFETIME,
		StringUtils::Null, true);
	ConfigureExtendedInterfacesImpl(config);
}


void ACBaseImpl::RegisterListener(const ApplicationContext::Listener& l) {
	if (executingFrameListeners)
		frameListenersToAdd.insert(l);
	else
		frameListeners.insert(l);
}

void ACBaseImpl::UnregisterListener(const ApplicationContext::Listener& l) {
	if (executingFrameListeners)
		frameListenersToRemove.insert(l);
	else
		frameListeners.erase(l);
}

bool ACBaseImpl::Step(Clock& clockTick) {

	if (frameListenersToAdd.size()) {
		// process any add requests
		frameListeners.insert(frameListenersToAdd.begin(),
			frameListenersToAdd.end());
		frameListenersToAdd.clear();
	}
	{
		executingFrameListeners = true;
		frameTimer.BeginFrame();
		WindowManager::Instance().ProcessMessages();
		// run the frame listeners
		for (auto& p : frameListeners)
			p.frameListener->Execute(frameTimer);

		frameTimer.EndFrame(clockTick.Tick());
		executingFrameListeners = false;
	}

	if (frameListenersToRemove.size()) {
		// process any removal requests
		FrameListenerSet::iterator it = frameListenersToRemove.begin();
		FrameListenerSet::iterator en = frameListenersToRemove.end();
		for (; it != en; ++it)
			frameListeners.erase(*it);
		frameListenersToRemove.clear();
	}
	return true;
}

void ACBaseImpl::DestroyServices() {
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

void ACBaseImpl::QuitApplication() {
	WindowManager::Instance().Quit();
}

/**************************************************************************
*                          ApplicationContextType
**************************************************************************/
String ApplicationContextType::ToString() const {
	switch (_type) {
	case CONTEXT_BASE:
		return "Base";
	case CONTEXT_ENGINE:
		return "Engine";
	case CONTEXT_PROJECT:
		return "Project";
	}
	return StringUtils::Unknown;
}

void ApplicationContextType::FromString(const String& name) {
	String n = name;
	StringUtils::ToLower(n);
	if (n == "base")
		_type = CONTEXT_BASE;
	else if (n == "engine")
		_type = CONTEXT_ENGINE;
	else if (n == "project")
		_type = CONTEXT_PROJECT;
	else
		_type = CONTEXT_UNKNOWN;
}

bool ApplicationContextType::IsAccepted(EApplicationContextType type) {
	if (type >= 0 && type <= _type)
		return true;
	return false;
}

}
