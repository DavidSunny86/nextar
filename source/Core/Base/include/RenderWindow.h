#ifndef NEXTAR_GRAPHICS_WINDOW_H
#define NEXTAR_GRAPHICS_WINDOW_H

#include <NexBase.h>
#include <NameValueMap.h>

namespace nextar {

	class _NexBaseExport RenderWindow {

		NEX_LOG_HELPER(RenderWindow);

	public:

		class EventListener {
		public:
			virtual void WindowConfigChanged(RenderWindow*) = 0;
			virtual void WindowCreated(RenderWindow*) = 0;
			virtual void WindowDestroying(RenderWindow*) = 0;
			virtual void WindowFocusChanged(RenderWindow*) = 0;
		};
		typedef list<EventListener*>::type WinEventListnerList;

	public:

		RenderWindow();
		virtual ~RenderWindow();


		/** @remarks Is Closed? */
		inline bool IsClosed() const {
			return (flags & WINDOW_CLOSED) != 0;
		}
		/** @remarks Is Active? */
		inline bool IsActive() const {
			return (flags & WINDOW_ACTIVE) != 0;
		}
		/** @remarks Is Visible? */
		inline bool IsVisible() const {
			return (flags & WINDOW_VISIBLE) != 0;
		}
		/* Is main window? */
		inline bool IsMainWindow() const {
			return (flags & WINDOW_IS_MAIN) != 0;
		}
		/* Is exit on close active */
		inline bool IsExitOnCloseEnabled() const {
			return (flags & WINDOW_EXIT_ON_CLOSE) != 0;
		}

		// set flag
		inline void SetFlag(uint32 flag, bool value) {
			flags = value ? flags | flag : flags & (~flag);
		}

		/** @remarks Add winodw event listener */
		virtual void AddListener(EventListener*);
		/** @remarks Remove winodw event listener */
		virtual void RemoveListener(EventListener*);

		// notifications received by this window from manager
		/** @remarks This is called by windows manager when
		 *           the window configuration changes
		 *           (includes width/height left/top).
		 *           Overrides should call this function after changing config.
		 */
		virtual void WindowConfigChanged();

		/** @remarks Called by WM to indicate window focus changed 
		 *           Overrides should call this function after changing focus.
		 */
		virtual void WindowFocusChanged();

		/** @remarks Called by WM to indicate window needs destruction */
		virtual void Destroy();

		/** @remarks Create an window if it does not exist */
		virtual void Create(uint32 width, uint32 height, bool fullscreen,
				const NameValueMap* params);

		/** @remarks Called by WM to indicate if window is visible */
		virtual void SetVisible(bool val);

		/** @remarks Called by WM to indicate if window is active */
		virtual void SetActive(bool act);

		/**
		 * @brief	Set to full screen
		 **/
		virtual void SetToFullScreen(bool fullscreen) = 0;

		virtual void DestroyImpl() = 0;

		virtual void CreateImpl(uint32 width, uint32 height, bool fullscreen,
				const NameValueMap* params) = 0;
	protected:
		enum {
			WINDOW_CREATED = 1 << 0,
			WINDOW_VISIBLE = 1 << 1,
			WINDOW_ACTIVE = 1 << 2,
			WINDOW_CLOSED = 1 << 3,
			WINDOW_IS_MAIN = 1 << 4,
			WINDOW_FULLSCREEN = 1 << 5,
			WINDOW_LAST_FLAG = 1 << 6,
			WINDOW_EXIT_ON_CLOSE = 1 << 7,
		};
				
		String windowTitle;
		uint32 flags;
		WinEventListnerList listeners;
	};

}

#if defined(NEX_LINUX)
#include <platform/linux/RenderWindowX.h>
#elif defined(NEX_WINDOWS)
#include <platform/win32/RenderWindowWin.h>
#else
#error Not supported
#endif

#endif //NEXTAR_GRAPHICS_WINDOW_H
