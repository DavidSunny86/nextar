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
		PRIORITY_LOW = 999999, PRIORITY_NORMAL = 500000, PRIORITY_HIGH = 100000,
	};

	class ErrorHandler {
	public:
		virtual void ShowErrorDialog(int errorCode, const String& desc) = 0;
	protected:
		~ErrorHandler() {}
	};

	struct Listener {
		FrameListener* frameListener;
		uint32 priority;

		Listener(FrameListener* listener, uint32 listenerPriority) :
				frameListener(listener), priority(listenerPriority) {
		}

		inline bool operator <(const Listener& other) const {
			return priority < other.priority;
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

private:

	NEX_THREAD_RECURSIVE_MUTEX(appLock);

	typedef set<Listener>::type FrameListenerSet;

	bool quitting;

	Clock frameClock;
	FrameTimer frameTimer;

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