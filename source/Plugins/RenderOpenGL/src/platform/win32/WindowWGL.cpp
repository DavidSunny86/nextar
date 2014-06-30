/*
 * GraphicsWindowGLX.cpp
 *
 *  Created on: 17-Aug-2013
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <RenderContextWGL.h>
#include <WindowWGL.h>

namespace RenderOpenGL {

ATOM WindowWGL::wndClass = 0;
void WindowWGL::InitializeWindowClass() {
	if (WindowWGL::wndClass)
		return;

	WNDCLASSEX wcx;
	HINSTANCE hinstance = GetModuleHandle(NULL);
	// Fill in the window class structure with parameters 
	// that describe the main window. 
	wcx.cbSize = sizeof (wcx); // size of structure 
	wcx.style = CS_HREDRAW |
			CS_VREDRAW; // redraw if size changes 
	wcx.lpfnWndProc = Win32Window::WndProc; // points to window procedure 
	wcx.cbClsExtra = 0; // no extra class memory 
	wcx.cbWndExtra = 0; // no extra window memory 
	wcx.hInstance = hinstance; // handle to instance 
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW); // predefined arrow 
	wcx.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = "NextarWindowWGL"; // name of window class 
	wcx.hIconSm = (HICON) LoadImage(hinstance, // small class icon
			MAKEINTRESOURCE(5),
			IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR);

	// Register the window class. 
	wndClass = RegisterClassEx(&wcx);
}

WindowWGL::WindowWGL(RenderContextWGL* ctx) :
context(ctx), nextar::Win32Window(NEX_NEW( WindowWGL::Impl(this))) {
}

WindowWGL::~WindowWGL() {
	NEX_DELETE(impl);
	impl = nullptr;
}

WindowWGL::Impl::Impl(WindowWGL* _parent) :
		parent(_parent) {
	ZeroMemory(pbo, sizeof(pbo));
}

void WindowWGL::Impl::Create(uint32 width, uint32 height,
bool fullscreen, const NameValueMap* params) {

	RenderContextWGL* context = parent->GetContext();
	position.x = 0;
	position.y = 0;
		
	HWND parentWindow = GetDesktopWindow();
	uint32 refreshRate = 0;

	parent->SetWindowTitle("NexTech: Render Window");
	uint32 setVideoMode = -1;

	if (params) {
		const NameValueMap& paramMap = *params;
		NameValueMap::const_iterator end = paramMap.end();
		NameValueMap::const_iterator attrib;
		if (((attrib = paramMap.find("WindowLeft")) != end))
			position.x = Convert::ToLong((*attrib).second);
		if (((attrib = paramMap.find("WindowTop")) != end))
			position.y = Convert::ToLong((*attrib).second);
		if (((attrib = paramMap.find("WindowTitle")) != end))
			parent->SetWindowTitle((*attrib).second);
		if (((attrib = paramMap.find("IsMainWindow")) != end))
			parent->SetFlag(WINDOW_IS_MAIN, Convert::ToBool((*attrib).second));
		if (((attrib = paramMap.find("VideoMode")) != end))
			setVideoMode = context->GetVideoModeIndex(
					VideoMode::FromString((*attrib).second));
		if (((attrib = paramMap.find("ExitOnClose")) != end))
			parent->SetFlag(WINDOW_EXIT_ON_CLOSE,
					Convert::ToBool((*attrib).second));
	}

	if (position.x < 0)
		position.x = 0;
	if (position.y < 0)
		position.y = 0;

	if (!parent->IsMainWindow())
		fullscreen = false;

	
	if (fullscreen && setVideoMode != -1) {
		context->SetVideoMode(setVideoMode);
	}

	DWORD style = WS_OVERLAPPEDWINDOW;
	DWORD  exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	
	WindowWGL::InitializeWindowClass();

	RECT windowRect;
	windowRect.left = (LONG) position.x;
	windowRect.right = (LONG) (position.x + width);
	windowRect.top = (LONG) position.y;
	windowRect.bottom = (LONG) (position.y + height);

	if (!AdjustWindowRectEx(&windowRect, style, FALSE, exStyle))
		NEX_THROW_FatalError(EXCEPT_INVALID_ARGS);

	this->dimensions.dx = (uint16)(windowRect.right - windowRect.left);
	this->dimensions.dy = (uint16)(windowRect.bottom - windowRect.top);
	
	// create basic window
	HWND wnd = CreateWindowEx(exStyle, (LPCSTR) wndClass,
		parent->GetTitle().c_str(),
		style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		windowRect.left, windowRect.top,
		width,
		height,
		parentWindow,
		NULL, NULL, NULL);

	if (wnd == NULL) {
		Error("Failed to create window");
		NEX_THROW_FatalError(EXCEPT_INVALID_ARGS);
	}

	parent->SetWindowHandle(wnd);
	hDC = GetDC(wnd);
	parent->SetWindowDC(hDC);
	
	if (!SetPixelFormat(hDC, context->GetPixelFormat(),
		context->GetPixelDescriptor())) {
		Error("Could not set pixel format!");
		NEX_THROW_FatalError(EXCEPT_INVALID_ARGS);
	}

	// SetToFullScreen now
	if (fullscreen) {
		SetToFullScreen(true);
	}

	ShowWindow(wnd, SW_SHOW);
    SetForegroundWindow(wnd);
    SetFocus(wnd);
    UpdateWindow(wnd);
    
	parent->SetFlag(WINDOW_CLOSED, false);
	parent->SetWindowProc(WindowWGL::WndProc);	
}

void WindowWGL::Impl::SetToFullScreen(bool fullScreen) {
	if (fullScreen == parent->IsFullScreen())
		return;

	RenderContextWGL* context = parent->GetContext();
	
	RECT windowRect;

	DWORD style;
	DWORD exStyle;

	if (fullScreen) {
		if (!parent->IsMainWindow()) {
			Warn("Window needs to be main window to set to fullscreen.");
			return;
		}
		parent->SetFlag(WINDOW_FULLSCREEN, true);
		// keep the current mode and switch
		// update the client area
		VideoMode mode = context->GetCurrentMode();
		fsSwitchSize = Size(mode.width, mode.height);
		this->dimensions.height = mode.height;
		this->dimensions.width = mode.width;

		windowRect.left = (LONG) 0;
		windowRect.right = (LONG) (0 + mode.width);
		windowRect.top = (LONG) 0;
		windowRect.bottom = (LONG) (0 + mode.height);

		style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		exStyle = WS_EX_APPWINDOW;

	} else {
		context->SetVideoMode((uint32)-1);
		fullScreen = false;

		this->dimensions = fsSwitchSize;
		
		windowRect.left = (LONG) position.x;
		windowRect.right = (LONG) (position.x + dimensions.dx);
		windowRect.top = (LONG) position.y;
		windowRect.bottom = (LONG) (position.y + dimensions.dy);

		style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	}

	if (!AdjustWindowRectEx(&windowRect,
			style,
			FALSE,
			exStyle))
		NEX_THROW_FatalError(EXCEPT_INVALID_ARGS);


	SetWindowLong(parent->GetWindowHandle(), GWL_STYLE, style);
	SetWindowLong(parent->GetWindowHandle(), GWL_EXSTYLE, exStyle);
	SetWindowPos(parent->GetWindowHandle(), HWND_NOTOPMOST,
			windowRect.left,
			windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			SWP_SHOWWINDOW
			);

	UpdateWindow(parent->GetWindowHandle());
	SetForegroundWindow(parent->GetWindowHandle());
	SetFocus(parent->GetWindowHandle());
}

void WindowWGL::Impl::Destroy() {
	RenderContextWGL* context = parent->GetContext();
	if (parent->IsFullScreen())
		SetToFullScreen(false);
		
	if (context->IsCurrentDC(GetWindowDC()))
		context->SetCurrentTarget(nullptr);
	
    if (!ReleaseDC(parent->GetWindowHandle(), parent->GetWindowDC())) {
        Error("Failed to destroy device context");
        NEX_THROW_FatalError(EXCEPT_INVALID_CALL);
    }

    if (!DestroyWindow(parent->GetWindowHandle())) {
        Error("Failed to destroy window");
        NEX_THROW_FatalError(EXCEPT_INVALID_CALL);
    }

    parent->SetWindowHandle(0);
    parent->SetWindowDC(0);
	hDC = 0;
    if (parent->IsMainWindow()) {
        // post quit message
        PostQuitMessage(0);
    }
}

void WindowWGL::Impl::ApplyChangedAttributes() {
	// todo
}

void WindowWGL::Impl::FocusChanged() {
	// todo
}

PixelFormat WindowWGL::Impl::GetPixelFormat() const {
	return PixelFormat::BGRA8;
}

void WindowWGL::Impl::Capture(RenderContext* rc, PixelBox& image,
		FrameBuffer frameBuffer) {
	RenderContextWGL* context = parent->GetContext();
	if (rc != (RenderContext*)context) {
		Error("Window created using a different context");
		return;
	}
	if (!context->IsCurrentDC(hDC))
		context->SetCurrentTarget(this);

	context->Capture(image, this, pbo, frameBuffer);
}

void WindowWGL::Impl::Reset(RenderContext* rc, Size size, PixelFormat format) {
	RenderContextWGL* context = parent->GetContext();
	if (parent->IsFullScreen())
		SetToFullScreen(false);
	RECT windowRect;
	windowRect.left = (LONG) position.x;
	windowRect.right = (LONG) (position.x + dimensions.dx);
	windowRect.top = (LONG) position.y;
	windowRect.bottom = (LONG) (position.y + dimensions.dy);
	DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	DWORD exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	if (!AdjustWindowRectEx(&windowRect,
			style,
			FALSE,
			exStyle))
		NEX_THROW_FatalError(EXCEPT_INVALID_ARGS);
	SetWindowPos(parent->GetWindowHandle(), HWND_NOTOPMOST,
			windowRect.left,
			windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			SWP_SHOWWINDOW
			);
	dimensions.width = size.dx;
	dimensions.height = size.dy;
	UpdateWindow(parent->GetWindowHandle());
}

void WindowWGL::Impl::Present(RenderContext* rc) {
	RenderContextWGL* context = parent->GetContext();
	if (rc != context) {
		Error("Window created using a different context");
		return;
	}
	context->Present(GetWindowDC());
}

}
