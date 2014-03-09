#ifndef NEX_APPLICATIONCONTEXT_H_
#define NEX_APPLICATIONCONTEXT_H_

#include <BaseHeaders.h>
#include <Singleton.h>
#include <AppImpl.h>
#include <Clock.h>
#include <FrameTimer.h>
#include <Config.h>
#include <URL.h>

namespace nextar {

	class _NexBaseExport ApplicationContext: public Singleton<ApplicationContext> {
		NEX_LOG_HELPER(ApplicationContext)
		;

	public:

		struct Listener {
			FrameListener* frameListener;
			uint32 priority;

			Listener(FrameListener* listener, uint32 listenerPriority
				) : frameListener(listener), priority(listenerPriority) {
			}
			
			inline bool operator < (const Listener& other) const {
				return priority < other.priority || (priority == other.priority && frameListener < other.frameListener);
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

		void SetQuitting(bool value);

		virtual void InitializeContext();
		virtual void DestroyContext();

		virtual void Run();

		virtual void AsyncRegisterListener(const Listener&);
		virtual void AsyncUnregisterListener(const Listener&);

	protected:
		void CreateServices();
		void DestroyServices();

		virtual void LoadConfiguration();
		virtual void SaveConfiguration() {}
		virtual void ConfigureServices();
		virtual void CloseServices();

		virtual void ConfigureExtendedInterfacesImpl() {
		}
		virtual void CreateExtendedInterfacesImpl() {
		}
		virtual void DestroyExtendedInterfacesImpl() {
		}

	private:

		NEX_THREAD_RECURSIVE_MUTEX(appLock);
		NEX_THREAD_RECURSIVE_MUTEX(addListener);
		NEX_THREAD_RECURSIVE_MUTEX(removeListener);

		typedef set<Listener>::type FrameListenerSet;

		bool quitting;

		Clock frameClock;
		FrameTimer frameTimer;

		/* configuration */
		Config config;
		/** Application specific information */
		URL defaultConfigPath;
		String appName;

		FrameListenerSet frameListenersToRemove;
		FrameListenerSet frameListenersToAdd;

		FrameListenerSet frameListeners;

		static String _configPathName;
	};

}

#endif //NEX_APPLICATIONCONTEXT_H_
