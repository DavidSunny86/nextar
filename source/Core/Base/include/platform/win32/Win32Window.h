#ifndef NEX_GRAPHICSWINDOW_H
#define NEX_GRAPHICSWINDOW_H

#include <IncWnds.h>
#include <RenderWindow.h>

namespace nextar {

class _NexBaseAPI Win32Window: public RenderWindow {
protected:
	HWND window;
	HDC window_dc;
	WNDPROC original_proc;
	bool full_control;

	NEX_LOG_HELPER(Win32Window);

public:

	inline Win32Window(Impl* impl) :
			RenderWindow(impl), window(0), full_control(false), original_proc(0) {
	}

	~Win32Window();

	WNDPROC GetWindowProc() {
		return original_proc;
	}

	void SetWindowProc(WNDPROC proc) {
		original_proc = proc;
	}

	HWND GetWindowHandle() {
		return window;
	}

	void SetWindowHandle(HWND wnd) {
		window = wnd;
	}

	HDC GetWindowDC() {
		return window_dc;
	}

	void SetWindowDC(HDC dc) {
		window_dc = dc;
	}

	/** @copydoc RenderWindow::WindowConfigChanged */
	virtual void WindowConfigChanged();

	static LRESULT CALLBACK WndProc(HWND wnd, UINT message, WPARAM wparam,
			LPARAM lparam);
};
}

#endif //NEX_GRAPHICSWINDOW_H
