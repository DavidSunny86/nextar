#ifndef NEX_APPLICATIONCONTEXT_H_
#define NEX_APPLICATIONCONTEXT_H_

#include <BaseHeaders.h>
#include <Singleton.h>
#include <AppImpl.h>
#include <Clock.h>
#include <FrameTimer.h>
#include <Config.h>
#include <URL.h>
#include <EventDispatcher.h>
#include <ApplicationContextType.h>

namespace nextar {

class _NexBaseAPI ApplicationContext:
	public EventDispatcher,
	public Singleton<ApplicationContext> {
	NEX_LOG_HELPER(ApplicationContext);

public:


	enum EventID {
		EVENT_INIT_RESOURCES,
		EVENT_DESTROY_RESOURCES,
		LAST_EVENT_ID,
	};

	enum ListnerPriority {
		PRIORITY_HIGH = 10000,
		PRIORITY_NORMAL = 50000,
		PRIORITY_LOW = 100000, 
		
	};

	class ErrorHandler {
	public:
		virtual void ShowErrorDialog(int errorCode, const String& desc) = 0;
	protected:
		~ErrorHandler() {}
	};

	struct Listener {

		uint32 priority;
		FrameListener* frameListener;
		
		static uint32 BeginFramePriority(uint32 subPriority) {
			return PRIORITY_HIGH + subPriority;
		}

		static uint32 EndFramePriority(uint32 subPriority) {
			return PRIORITY_LOW + subPriority;
		}

		Listener(FrameListener* listener, uint32 listenerPriority) :
				frameListener(listener), priority(listenerPriority) {
		}
		
		inline bool operator <(const Listener& other) const {
			return (priority < other.priority) || 
				(priority == other.priority && frameListener < other.frameListener);
		}
	};

	class Impl : public AllocGeneral {
	public:
		virtual ~Impl() {}

		virtual ApplicationContextType GetType() const = 0;
		virtual void CreateServices() = 0;
		virtual void ConfigureServices(Config& ) = 0;
		virtual void ReleaseResources() = 0;
		virtual void DestroyServices() = 0;
		virtual bool Step(Clock& clockTick) = 0;
		virtual void RegisterListener(const Listener&) = 0;
		virtual void UnregisterListener(const Listener&) = 0;
		virtual void QuitApplication() = 0;
		virtual void LoadConfiguration() = 0;
		virtual void SaveConfiguration() = 0;
		virtual void ParseCommandLine(const NameValueMap& params) = 0;
		
	};

	
	ApplicationContext(const String& name, Impl& pImpl);
	virtual ~ApplicationContext(void);

	inline ApplicationContextType GetType() const {
		return _pImpl.GetType();
	}

	inline String GetAppName() const {
		return appName;
	}

	inline void SetAppName(const String& name) {
		appName = name;
	}

	inline Config& GetConfig() {
		return config;
	}

	inline const Config& GetConfig() const {
		return config;
	}

	/* This function should be called to exit the running application */
	void QuitApplication();
	/* @nodoc */
	void SetQuitting(bool value);
	
	void ParseCommandLineParams(int argc, char* argv[]);
	virtual void InitializeContext(int argc, char* argv[]);
	virtual void DestroyContext();
	virtual void ReleaseResources();

	virtual void Run();

	virtual void RegisterListener(const Listener&);
	virtual void UnregisterListener(const Listener&);

	virtual void ShowErrorDialog(int errorCode, const String& errorText);

protected:
	
	void DestroyServices();

	virtual void LoadConfiguration();
	virtual void SaveConfiguration();
	

protected:

	
private:

	NEX_THREAD_RECURSIVE_MUTEX(appLock);
		

	Impl& _pImpl;

	bool quitting;

	Clock frameClock;
	/* configuration */
	Config config;
	/** Application specific information */
	URL defaultConfigPath;
	String appName;

	bool _resourcesDestroyed;
	bool runningLoop;
	// @todo Add error handler
	ErrorHandler* errorHandler;
	static String _configPathName;
};

class _NexBaseAPI ACBaseImpl : public ApplicationContext::Impl
{
public:

	ACBaseImpl() : executingFrameListeners(false) {}

	virtual ApplicationContextType GetType() const;
	virtual void CreateServices();
	virtual void ConfigureServices(Config& config);
	virtual bool Step(Clock& clockTick);
	virtual void ReleaseResources() {}
	virtual void RegisterListener(const ApplicationContext::Listener&);
	virtual void UnregisterListener(const ApplicationContext::Listener&);

	virtual void DestroyServices();

	virtual void LoadConfiguration() {}
	virtual void SaveConfiguration() {}
	virtual void ParseCommandLine(const NameValueMap& params) {}

	virtual void ConfigureExtendedInterfacesImpl(Config& config) {}
	virtual void CreateExtendedInterfacesImpl() {}
	virtual void DestroyExtendedInterfacesImpl() {}
	

	virtual void QuitApplication();
protected:

	typedef set<ApplicationContext::Listener>::type FrameListenerSet;

	FrameTimer frameTimer;

	bool executingFrameListeners;

	FrameListenerSet frameListenersToRemove;
	FrameListenerSet frameListenersToAdd;
	FrameListenerSet frameListeners;

};



}

#endif //NEX_APPLICATIONCONTEXT_H_