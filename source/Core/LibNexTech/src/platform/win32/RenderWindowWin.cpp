
#include "NexHeaders.h"
#include "RenderWindow.h"

#if defined(NEX_WINDOWS)

namespace nextar {

    void RenderWindowWin::WindowConfigChanged() {
        // todo get the resized window
        GraphicsWindow::WindowConfigChanged();
    }

	LRESULT CALLBACK RenderWindowWin::WndProc(HWND wnd, UINT message, WPARAM wparam,
			LPARAM lparam) {

		RenderWindow* wind = (RenderWindow*) GetWindowLongPtr(wnd,
				GWLP_USERDATA);

		switch (message) {
		case WM_CLOSE:
			if (!wind->IsClosed()) {
				wind->Destroy();
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
				WmInputListener::Instance().OnWmInput((HRAWINPUT) lparam);
			break;
		}

		return DefWindowProc(wnd, message, wparam, lparam);
	}

}


#endif