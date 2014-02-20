#include "NexHeaders.h"
#include "WindowManager.h"

#if defined(NEX_LINUX)
#include <X11/X.h>
#include "platform/linux/RenderWindowX.h"
#elif defined(NEX_WINDOWS)
#include <windows.h>
#include "platform/win32/RenderWindowWin.h"
#include "platform/win32/WmInputListener.h"
#else
#error No WM yet for this platform
#endif

namespace nextar {

	NEX_DEFINE_SINGLETON_PTR(WindowManager);

	WindowManager::WindowManager() : processing(false) {
	}

	WindowManager::~WindowManager() {
	}

	void WindowManager::RegisterWindow(RenderWindowPtr& winPtr) {
#if defined( NEX_WINDOWS )
		RenderWindowWin* window = static_cast<RenderWindowWin*>(winPtr.GetPtr());
		HWND wndH = window->GetWindowHandle();
		SetWindowLongPtr(wndH, GWLP_USERDATA,
				reinterpret_cast<LONG_PTR>(window));

		WNDPROC proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(wndH,
				GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&RenderWindowWin::WndProc)));
		window->SetWindowProc(proc);

#endif
		registeredWindows.push_back(winPtr);
	}

	void WindowManager::_Unregister(RenderWindowPtr& winPtr) {
		registeredWindows.remove( winPtr);
#if defined( NEX_WINDOWS )
		RenderWindowWin* window = static_cast<RenderWindowWin*>(winPtr.GetPtr());
		HWND wndH = window->GetWindowHandle();
		SetWindowLongPtr(wndH, GWLP_WNDPROC,
				reinterpret_cast<LONG_PTR>(window->GetWindowProc()));
#endif
	}

	void WindowManager::UnregisterWindow(RenderWindowPtr& winPtr) {
		if (processing)
			removedWindows.push_back(winPtr);
		else
			_Unregister(winPtr);
	}

	void WindowManager::ProcessMessages() {
		processing = true;
#if defined(NEX_WINDOWS)
		static MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				ApplicationContext::Instance().SetQuitting(true);
			}
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

#elif defined(NEX_LINUX)
		// for all registered windows
		XEvent event;
		WindowList::iterator it = registeredWindows.begin();
		WindowList::iterator end = registeredWindows.end();
		while (it != end) {
			RenderWindowX* wind = static_cast<RenderWindowX*>((*it).GetPtr());
			Display* disp = wind->GetDisplay();
			Window w = wind->GetWindow();
			while (XCheckWindowEvent(
							disp,
							w,
							StructureNotifyMask | VisibilityChangeMask | FocusChangeMask,
							&event)) {
				RenderWindowX::ProcessEvent(wind, event);
			}

			// The ClientMessage event does not appear under any Event Mask
			while (XCheckTypedWindowEvent(disp, w, ClientMessage, &event)) {
				RenderWindowX::ProcessEvent(wind, event);
			}
			++it;
		}
#else
#error Not supported
#endif
		processing = false;
		if (removedWindows.size()) {
			WindowList::iterator rwit = removedWindows.begin();
			WindowList::iterator rwend = removedWindows.end();
			while (rwit != rwend) {
				RenderWindowPtr wp = (*rwit);
				_Unregister(wp);
				++rwit;
			}
			removedWindows.clear();
		}

	}

}

