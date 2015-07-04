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

namespace nextar {

class _NexBaseAPI ApplicationContext:
	public EventDispatcher,
	public Singleton<ApplicationContext> {
	NEX_LOG_HELPER(ApplicationContext)
	;

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

	ApplicationContext(const String& name);
	virtual ~ApplicationContext(void);

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
	
	void ParseCommandLineParaqms(int argc, char* argv[]);
	virtual void InitializeContext(int argc, char* argv[]);
	virtual void DestroyContext();
	virtual void ReleaseResources();

	virtual void Run();

	virtual void RegisterListener(const Listener&);
	virtual void UnregisterListener(const Listener&);

	virtual void ShowErrorDialog(int errorCode, const String& errorText);

protected:
	void CreateServices();
	void DestroyServices();

	virtual void LoadConfiguration();
	virtual void SaveConfiguration() {
	}
	virtual void ConfigureServices();

	virtual void ConfigureExtendedInterfacesImpl() {
	}
	virtual void CreateExtendedInterfacesImpl() {
	}
	virtual void DestroyExtendedInterfacesImpl() {
	}
	virtual void ReleaseResourcesImpl() {
	}
	virtual void ParseCommandLine(const NameValueMap& params) {
	}

protected:

	FrameTimer frameTimer;

private:

	NEX_THREAD_RECURSIVE_MUTEX(appLock);

	typedef set<Listener>::type FrameListenerSet;

	bool quitting;

	Clock frameClock;
	/* configuration */
	Config config;
	/** Application specific information */
	URL defaultConfigPath;
	String appName;

	bool resourcesDestroyed;
	bool runningLoop;
	FrameListenerSet frameListenersToRemove;
	FrameListenerSet frameListenersToAdd;
	FrameListenerSet frameListeners;
	// @todo Add error handler
	ErrorHandler* errorHandler;
	static String _configPathName;
};

}

#endif //NEX_APPLICATIONCONTEXT_H_