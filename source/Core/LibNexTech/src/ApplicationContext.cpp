
#include "NexHeaders.h"
#include "ApplicationContext.h"
#include "SharedObjectTracker.h"
#include "FrameListener.h"
#include "PluginRegistry.h"
#include "FileSystem.h"
#include "RenderManager.h"
#include "WindowManager.h"
#include "PropertyInterface.h"
#include "ComponentManagerArchive.h"
#include "Mesh.h"

namespace nextar {

	NEX_DEFINE_SINGLETON_PTR(ApplicationContext);

	String ApplicationContext::_configPathName = "${Bin}/Config.cfg";

	ApplicationContext::ApplicationContext(const String& name) :
			appName(name),
			quitting(false) {
		NEX_NEW LogManager();
#ifdef NEX_DEBUG
		// add a default logger
		static Logger logger("Debug");
		LogManager::Instance().AddNewLogger(&logger, true);
#endif
	}

	ApplicationContext::~ApplicationContext(void) {
		NEX_DELETE LogManager::InstancePtr();
	}

	void ApplicationContext::InitializeContext() {
		Trace("Initializing application context: " + appName);
		CreateServices();
		CreateComponentManagers();
		LoadConfiguration();
		// load sub-plugins
		PluginRegistry::Instance().RequestPlugins(PLUG_TYPE_LIFETIME, StringUtils::Null, true);

		ConfigureServices();
		ConfigureComponentManagers();
		ConfigureExtendedInterfacesImpl();
	}

	void ApplicationContext::LoadConfiguration() {
		// this is absolutely necessary that we do it here
		// as file system is needed to resolve URLs
		defaultConfigPath = URL(_configPathName);
		try {
			config.LoadConfiguration(defaultConfigPath);
			// ready filesystem
			FileSystem::Instance().Configure(config);
			// load plugins
			PluginRegistry::Instance().Configure(config);
		} catch (GracefulErrorExcept& ex) {
			Error(ex.GetMsg());
		}
	}

	void ApplicationContext::ConfigureServices() {
		// if render manager is present, configure it
		if (RenderManager::InstancePtr())
			RenderManager::Instance().Configure(config);
	}

	void ApplicationContext::CloseServices() {
		// if render manager is present, close it
		if (RenderManager::InstancePtr())
			RenderManager::Instance().Close();
	}

	void ApplicationContext::DestroyContext() {
		Trace("Destroying application context: " + appName);
		SaveConfiguration();
		// destroy managers
		CloseComponentManagers();
		// destroy dictionaries
		PropertyInterface::DestroyDictionaries();
		// close services
		CloseServices();
		// unload plugins
		PluginRegistry::Instance().RequestPlugins(PLUG_TYPE_LIFETIME, StringUtils::Null, false);
		// services
		DestroyServices();
	}

	void ApplicationContext::CreateServices() {
		NEX_NEW FileSystem();
		NEX_NEW PluginRegistry();
		//NEX_NEW WindowManager();
		NEX_NEW ComponentManagerArchive();

		CreateExtendedInterfacesImpl();
	}

	void ApplicationContext::DestroyServices() {

		DestroyExtendedInterfacesImpl();

		NEX_ASSERT (!RenderManager::InstancePtr());

		NEX_DELETE ComponentManagerArchive::InstancePtr();
		//NEX_DELETE WindowManager::InstancePtr();
		NEX_DELETE PluginRegistry::InstancePtr();
		NEX_DELETE FileSystem::InstancePtr();
	}

	void ApplicationContext::SetQuitting(bool value) {
		quitting = value;
	}

	void ApplicationContext::Run() {

		frameClock.StartClock();

		while (!quitting) {
			frameClock.Tick();
			{
				NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(addListener);
				// process any add requests
				frameListeners.insert(frameListenersToAdd.begin(), frameListenersToAdd.end());
				frameListenersToAdd.clear();
			}
			{
				frameTimer.BeginFrame();
				WindowManager::Instance().ProcessMessages();
				// run the frame listeners
				FrameListenerSet::iterator it = frameListeners.begin();
				FrameListenerSet::iterator en = frameListeners.end();
				for(; it != en; ++it)
					(*it).frameListener->BeginFrame(frameTimer.GetFrameNumber());

				uint32 elapsedMilSec = frameClock.Tick();

				it = frameListeners.begin();
				for(; it != en; ++it)
					(*it).frameListener->EndFrame(elapsedMilSec);
				frameTimer.EndFrame(elapsedMilSec);
			}

			{
				NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(removeListener);
				// process any removal requests
				FrameListenerSet::iterator it = frameListenersToRemove.begin();
				FrameListenerSet::iterator en = frameListenersToRemove.end();
				for(; it != en; ++it)
					frameListeners.erase(*it);
				frameListenersToRemove.clear();
			}
		}
		frameClock.StopClock();
	}

	void ApplicationContext::AsyncRegisterListener(const Listener& l) {
		NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(addListener);
		frameListenersToAdd.insert(l);
	}

	void ApplicationContext::AsyncUnregisterListener(const Listener& l) {
		NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(removeListener);
		frameListenersToRemove.insert(l);
	}

	void ApplicationContext::CreateComponentManagers() {
		/*@todo Optimize by sending a list*/
		/* default mesh manager */
		MeshManager* mshM = NEX_NEW MeshManager("Mesh");
		ComponentManagerArchive::Instance().AsyncAddManager(Mesh::TYPE, mshM);
		/* default shader manager */
		/* default material manager */
		MaterialManager* mtlM = NEX_NEW MaterialManager("Material");
		ComponentManagerArchive::Instance().AsyncAddManager(Material::TYPE, mtlM);
	}

	void ApplicationContext::ConfigureComponentManagers() {
		ComponentManagerArchive::Instance().Configure(config);
	}

	void ApplicationContext::CloseComponentManagers() {
		ComponentManagerArchive::Instance().AsyncDeleteAll();
	}
}
