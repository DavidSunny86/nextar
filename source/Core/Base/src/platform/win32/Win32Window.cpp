#include <BaseHeaders.h>
#include <RenderWindow.h>

#if defined(NEX_WINDOWS)

#include <Win32Window.h>
#include <WmInputListener.h>

namespace nextar {

void Win32Window::WindowConfigChanged() {
	// todo get the resized window
	RenderWindow::WindowConfigChanged();
}

LRESULT CALLBACK Win32Window::WndProc(HWND wnd, UINT message, WPARAM wparam,
	LPARAM lparam) {

	Win32Window* wind = (Win32Window*) GetWindowLongPtr(wnd,
		GWLP_USERDATA);
	if (wind) {
		switch (message) {
		case WM_CLOSE:
			if (!wind->IsClosed()) {
				wind->Destroy();
			}
			if (wind->IsMainWindow()) {
				// post quit message
				PostQuitMessage(0);
			}
			break;
		case WM_EXITSIZEMOVE:
			// todo should have size change started, size change ended
			wind->WindowConfigChanged();
			break;
		case WM_SIZE:
			// For full screen window, this can
			// be a toggle, else a resize
			if (wparam == SIZE_MINIMIZED) {
				wind->SetActive(false);
				wind->SetVisible(false);
			} else {
				wind->SetActive(true);
				wind->SetVisible(true);
			}
			wind->WindowConfigChanged();
			wind->WindowFocusChanged();
			break;
		case WM_ACTIVATE:
			// Add pause code here.
			if (WA_INACTIVE == LOWORD(wparam)) {
				wind->SetActive(false);
				wind->SetVisible(false);
			} else {
				wind->SetActive(true);
				wind->SetVisible(true);
			}
			wind->WindowFocusChanged();
			break;
		case WM_INPUT:
			if (WmInputListener::InstancePtr() && wparam == RIM_INPUT)
				WmInputListener::Instance().OnWmInput(wind, (HRAWINPUT) lparam);
			break;
		}
	}
	return DefWindowProc(wnd, message, wparam, lparam);
}

}

#endif
