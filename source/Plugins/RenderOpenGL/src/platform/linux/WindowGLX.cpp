/*
 * GraphicsWindowGLX.cpp
 *
 *  Created on: 17-Aug-2013
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <RenderContextImplGLX.h>

namespace RenderOpenGL {

WindowGLX::WindowGLX(RenderContextImplGLX* ctx) :
context(ctx), nextar::XWindow(NEX_NEW( WindowGLX::Impl(this))) {
	static_cast<WindowGLX::Impl*>(impl)->AddRef();
}

WindowGLX::~WindowGLX() {
	static_cast<WindowGLX::Impl*>(impl)->Release();
	impl = nullptr;
}

WindowGLX::Impl::Impl(WindowGLX* _parent) :
		parent(_parent), drawable(0), cmap(0), pbo{ 0 } {
}

void WindowGLX::Impl::Create(uint32 width, uint32 height,
bool fullscreen, const NameValueMap* params) {

	RenderContextImplGLX* context = parent->GetContext();
	position.x = 0;
	position.y = 0;

	Display* display = context->GetDisplay();
	parent->SetDisplay(display);

	Window parentWindow = RootWindow(display,
			context->GetScreenIndex());
	parent->SetWindowTitle("NexTech: Render Window");
	uint32 setVideoMode = -1;
	bool captureInputEvents = true;

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
		if (((attrib = paramMap.find("IgnoreInputEvents")) != end))
			captureInputEvents = !Convert::ToBool((*attrib).second);
		if (((attrib = paramMap.find("VideoMode")) != end))
			setVideoMode = context->GetContext()->GetVideoModeIndex(
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

	this->dimensions.dx = width;
	this->dimensions.dy = height;

	if (fullscreen && setVideoMode != -1) {
		context->GetContext()->SetVideoMode(setVideoMode);
	}

	GLXFBConfig fbconfig = context->GetFrameBufferConfig();
	XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbconfig);

	// X Win Attributes
	XSetWindowAttributes swa;
	uint32 mask;
	swa.colormap = cmap = XCreateColormap(display,
			RootWindow(display, vi->screen), vi->visual, AllocNone);
	swa.background_pixel = 0;
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask | VisibilityChangeMask
			| FocusChangeMask;
	if (captureInputEvents)
		swa.event_mask |= (KeyPressMask | KeyReleaseMask |
				ButtonPressMask | ButtonReleaseMask
				| PointerMotionMask);

	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	Window window = XCreateWindow(display, parentWindow, position.x, position.y,
			width, height, 0, vi->depth,
			InputOutput, vi->visual, mask, &swa);

	XFree(vi);

	if (!window) {
		// error
		Error("Failed to create window.");
		NEX_THROW_FatalError(EXCEPT_INVALID_CALL);
	}

	parent->SetWindow(window);
	parent->SetEventMask(swa.event_mask);

	Atom destroyMsg = XInternAtom(display, "WM_DELETE_WINDOW", False);
	parent->SetDestroyMsg(destroyMsg);
	// trap delete
	XSetWMProtocols(display, window, &destroyMsg, 1);

	// SetToFullScreen now
	if (fullscreen) {
		SetToFullScreen(true);
	}

	XStoreName(display, window, parent->GetTitle().c_str());
	XMapWindow(display, window);

	parent->SetFlag(WINDOW_CLOSED, false);

	XFlush (display);
	drawable = window;
}

void WindowGLX::Impl::SetToFullScreen(bool fullScreen) {
	RenderContextImplGLX* context = parent->GetContext();
	Display* display = parent->GetDisplay();
	Window window = parent->GetWindow();
	if (fullScreen) {

		if (!parent->IsMainWindow()) {
			Warn("Window needs to be main window to set to fullscreen.");
			return;
		}
		parent->SetFlag(WINDOW_FULLSCREEN, true);
		// keep the current mode and switch
		// update the client area
		VideoMode mode = context->GetContext()->GetCurrentMode();
		dimensions.height = mode.height;
		dimensions.width = mode.width;
		context->SwitchToFullScreen(window, true);
	} else {
		XWindowAttributes attribs;
		parent->SetFlag(WINDOW_FULLSCREEN, false);
		Display* display = context->GetDisplay();
		context->SwitchToFullScreen(window, false);
		XGetWindowAttributes(display, window, &attribs);
		dimensions.width = attribs.width;
		dimensions.height = attribs.height;
	}
}

void WindowGLX::Impl::Destroy() {
	RenderContextImplGLX* context = parent->GetContext();
	Display* display = parent->GetDisplay();
	Window window = parent->GetWindow();


	if (context->IsCurrentDrawable(drawable))
		context->GetContext()->SetCurrentTarget(nullptr);
	XDestroyWindow(display, window);
	window = 0;
	XFreeColormap(display, cmap);
	cmap = 0;
	context->GetContext()->DestroyedRenderWindow(parent);
}

void WindowGLX::Impl::ApplyChangedAttributes() {
	// todo
}

void WindowGLX::Impl::FocusChanged() {
	// todo
}

PixelFormat WindowGLX::Impl::GetPixelFormat() const {
	return PixelFormat::BGRA8;
}

void WindowGLX::Impl::Capture(RenderContext* rc, PixelBox& image,
		FrameBuffer frameBuffer) {
	RenderContextImplGLX* context = parent->GetContext();
	if (rc != (RenderContext*)context) {
		Error("Window created using a different context");
		return;
	}
	if (!context->IsCurrentDrawable(drawable))
		context->GetContext()->SetCurrentTarget(this);

	context->GetContext()->Capture(image, this, pbo, frameBuffer);
}

void WindowGLX::Impl::Reset(RenderContext* rc, Size size, PixelFormat format) {
	RenderContextImplGLX* context = parent->GetContext();
	Display* display = parent->GetDisplay();
	Window window = parent->GetWindow();
	if (parent->IsFullScreen())
		SetToFullScreen(false);
	XWindowAttributes attribs;
	XResizeWindow(display, window, size.dx, size.dy);
	dimensions.width = size.dx;
	dimensions.height = size.dy;
}

void WindowGLX::Impl::Present(RenderContext* rc) {
	RenderContextImplGLX* context = parent->GetContext();
	if (rc != context->GetContext()) {
		Error("Window created using a different context");
		return;
	}
	context->Present(this);
}

}
