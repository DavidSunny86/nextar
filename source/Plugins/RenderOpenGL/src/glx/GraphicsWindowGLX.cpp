/*
 * GraphicsWindowGLX.cpp
 *
 *  Created on: 17-Aug-2013
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <glx/GraphicsWindowGLX.h>
#include <glx/RenderContextGLX.h>

namespace RenderOpenGL {
	GraphicsWindowGLX::GraphicsWindowGLX(RenderContextGLX* ctx) :
		context(ctx), width(0), height(0), left(0), top(0), cmap(0), canvas(this, "Window") {
	}

	GraphicsWindowGLX::~GraphicsWindowGLX() {
	}

	GraphicsWindowGLX::WindowCanvas::WindowCanvas(GraphicsWindowGLX* _parent, const nextar::String& name) : CanvasGLX(name, true),
			parent(_parent) {
	}

	void GraphicsWindowGLX::CreateImpl(
	        uint32 width, uint32 height,
	        bool fullscreen,
	        const NameValueMap* params) {

		left = 0;
	    top = 0;

	    display = context->GetDisplay();
	    Window parentWindow = RootWindow(display, context->GetScreenIndex());
	    windowTitle = "NexTech: Render Window";
	    uint32 setVideoMode = -1;

	    if (params) {
	    	const NameValueMap& paramMap = *params;
	        NameValueMap::const_iterator end = paramMap.end();
	        NameValueMap::const_iterator attrib;
	        if (((attrib = paramMap.find("WindowLeft")) != end))
	            left = Convert::ToLong((*attrib).second);
	        if (((attrib = paramMap.find("WindowTop")) != end))
	            top = Convert::ToLong((*attrib).second);
	        if (((attrib = paramMap.find("WindowTitle")) != end))
	            windowTitle = (*attrib).second;
	        if (((attrib = paramMap.find("IsMainWindow")) != end))
	        	SetFlag(WINDOW_IS_MAIN, Convert::ToBool((*attrib).second));
	        if (((attrib = paramMap.find("VideoMode")) != end))
	        	setVideoMode = context->GetVideoModeIndex(VideoMode::FromString((*attrib).second));
	        if (((attrib = paramMap.find("ExitOnClose")) != end))
	        	SetFlag(WINDOW_EXIT_ON_CLOSE, Convert::ToBool((*attrib).second));
	    }

	    if (left < 0)
	        left = 0;
	    if (top < 0)
	        top = 0;

	    if (!IsMainWindow())
	    	fullscreen = false;

	    this->width = width;
	    this->height = height;

	    if (fullscreen && setVideoMode != -1) {
	        context->SetVideoMode(setVideoMode);
	    }

	    GLXFBConfig fbconfig = context->GetFrameBufferConfig();
	    XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbconfig);

	    // X Win Attributes
	    XSetWindowAttributes swa;
	    uint32 mask;
	    swa.colormap = cmap = XCreateColormap(display,
	            RootWindow(display, vi->screen),
	            vi->visual, AllocNone);
	    swa.background_pixel = 0;
	    swa.border_pixel = 0;
	    swa.event_mask = StructureNotifyMask | VisibilityChangeMask | FocusChangeMask;
	    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	    window = XCreateWindow(display, parentWindow,
	            left, top, width, height, 0, vi->depth,
	            InputOutput,
	            vi->visual,
	            mask, &swa);

	    XFree(vi);

	    if (!window) {
	        // error
	        Error("Failed to create window.");
	        NEX_THROW_FatalError(EXCEPT_INVALID_CALL);
	    }

	    this->msgDestroy = XInternAtom(display, "WM_DELETE_WINDOW", False);
	    // trap delete
	    XSetWMProtocols(display, window, &msgDestroy, 1);

	    // SetToFullScreen now
	    if (fullscreen) {
	        SetToFullScreen(true);
	    }

	    XStoreName(display, window, windowTitle.c_str());
	    XMapWindow(display, window);

	    flags &= ~WINDOW_CLOSED;

	    XFlush(display);
	    canvas.SetDrawable(GetDrawable());
	}

	void GraphicsWindowGLX::SetToFullScreen(bool fullScreen) {
		if (fullScreen) {

			if (!IsMainWindow()) {
				Warn("Window needs to be main window to set to fullscreen.");
				return;
			}
			SetFlag(WINDOW_FULLSCREEN, true);
			// keep the current mode and switch
			// update the client area
			VideoMode mode = context->GetCurrentMode();
		    height = mode.height;
		    width = mode.width;
		    context->SwitchToFullScreen(window, true);
		} else {
			XWindowAttributes attribs;
			SetFlag(WINDOW_FULLSCREEN, false);
			Display* display = context->GetDisplay();
			context->SwitchToFullScreen(window, false);
			XGetWindowAttributes(display, window, &attribs);
			width = attribs.width;
			height = attribs.height;
		}
	}

	void GraphicsWindowGLX::DestroyImpl() {
		if (context->IsCurrentDrawable(GetDrawable()))
			context->SetCurrentCanvas(0);
		XDestroyWindow(display, window);
		window = 0;
		XFreeColormap(display, cmap);
		cmap = 0;
	}
}
