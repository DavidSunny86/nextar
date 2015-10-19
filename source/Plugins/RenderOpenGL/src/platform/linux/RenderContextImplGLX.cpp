/*
 * RenderContextImplGLX.cpp
 *
 *  Created on: 11-Aug-2013
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <RenderDriverGL.h>
#include <RenderContextImplGLX.h>
#include <WindowGLX.h>
#include <X11/extensions/Xrandr.h>

namespace RenderOpenGL {

RenderContextImplGLX::RenderContextImplGLX(RenderContext_Base_GL* _driver) :
		RenderContext_Base_GL::PlatformImpl(_driver), display(0), context(0), screenIndex(0), motif(
				0), wmState(0), fullScreen(0), frameBufferConfig(0), GlXCreateContextAttribsARB(
				0), currentDrawable(0), xrandrSupported(false) {
}

RenderContextImplGLX::~RenderContextImplGLX() {
}

void RenderContextImplGLX::PostCloseImpl() {
	UnreadyContext();
}

void RenderContextImplGLX::UnreadyContext() {
	if (context) {
		baseContext->SetCurrentTarget(0);
		Trace("Destroying render context.");
		glXDestroyContext(display, context);
		context = 0;
	}
	CloseDisplay();
}

void RenderContextImplGLX::CloseDisplay() {
	XCloseDisplay(display);
	display = 0;
}

void RenderContextImplGLX::SetCreationParams(
		const RenderDriver::ContextCreationParams& contextCreationParams) {
	bool failed = true;
	baseContext->SetContextParams(contextCreationParams);
	display = OpenDisplay(((RenderDriverGL*) baseContext->GetDriver())->GetGpuIndex());
	if (display) {

		int error_base, event_base;
		if (glXQueryExtension(display, &error_base, &event_base) == True) {
			/* query version */
			int major = 0, minor = 0;
			glXQueryVersion(display, &major, &minor);
			if ((major >= 1 || (major == 1 && minor > 3))) {
				// get screen from screen index

				if (!contextCreationParams.monitorIndex) {
					screenIndex = DefaultScreen(display);
				} else {
					int screenCount = ScreenCount(display);
					if (contextCreationParams.monitorIndex < screenCount) {
						screenIndex = (int) contextCreationParams.monitorIndex;
					} else
						screenIndex = DefaultScreen(display);
				}

				VideoModeList vidModes;
				EnumVideoModes(vidModes);
				baseContext->SetVideoModes(std::move(vidModes));
				// check atoms

				fullScreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN",
						True);
				if (fullScreen != None) {
					wmState = XInternAtom(display, "_NET_WM_STATE", True);
				} else {
					wmState = None;
					// motif based?
					motif = XInternAtom(display, "_MOTIF_WM_HINTS", True);
				}

				failed = false;
			} else
				Error("Requires atleast GLX 1.3");
		} else
			Error("Requires atleast GLX extensions");

	}

	// choose fbconfig
	int preferredAttribs[] = {
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER, True, /* Request a double-buffered color buffer */
		GLX_SAMPLES, contextCreationParams.multiSamples,
		GLX_STENCIL_SIZE, contextCreationParams.stencilBits,
		GLX_DEPTH_SIZE, contextCreationParams.depthBits,
		GLX_RED_SIZE, 8, /* the maximum number of bits per component    */
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		None, None,
		None
	};

	int attrib = 18;
	if (contextCreationParams.stereo) {
		preferredAttribs[attrib++] = GLX_STEREO;
		preferredAttribs[attrib++] = GL_TRUE;
	}

	int baseAttribs[] = {
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
			GLX_RENDER_TYPE, GLX_RGBA_BIT,
			GLX_DOUBLEBUFFER, True, /* Request a double-buffered color buffer */
			GLX_RED_SIZE, 1,
			GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE, 1,
			None
	};

	frameBufferConfig = GetConfig(baseAttribs, preferredAttribs);

	// lets check for glx extensions
	ReadyGlxExtensions();

	if (failed)
		NEX_THROW_FatalError(EXCEPT_DEVICE_CREATION_FAILED);
}

void RenderContextImplGLX::ReadyGlxExtensions() {
	const char *glxExts = glXQueryExtensionsString(display, screenIndex);

	if (ExtensionHelper::IsSupported("GLX_ARB_create_context", glxExts))
		GlXCreateContextAttribsARB =
				(PFNGLXCREATECONTEXTATTRIBSARB) RenderContext_Base_GL::GetExtension(
						"glXCreateContextAttribsARB");
}

void RenderContextImplGLX::ReadyContext(RenderWindow* gw) {
	// create context based of fbConfig and set as current
	// do we have a shared context?
	GLXContext shared = 0;
	const RenderDriver::ContextCreationParams& contextCreationParams = baseContext->GetContextParams();
	if (contextCreationParams.sharedContextIndex >= 0) {
		RenderContextPtr ptr = baseContext->GetDriver()->AsyncGetContext(
				contextCreationParams.sharedContextIndex);
		if (ptr) {
			shared = static_cast<RenderContextImplGLX*>((
					static_cast<RenderContext_Base_GL*>(
							ptr.GetPtr()))->GetImpl())->context;
		}
	}

	bool result = true;
	// int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler([&result](Display*, XErrorEvent*) { result = false; return 0; } );
	if (!GlXCreateContextAttribsARB) {
		Trace("Creating old style context");
		context = glXCreateNewContext(display, frameBufferConfig,
		GLX_RGBA_TYPE, shared, True);
	} else {
		int contextAttribs[] = { GLX_CONTEXT_MAJOR_VERSION_ARB,
				contextCreationParams.reqOpenGLVersionMajor,
				GLX_CONTEXT_MINOR_VERSION_ARB,
				contextCreationParams.reqOpenGLVersionMinor,
#ifdef NEX_DEBUG
				GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_DEBUG_BIT_ARB,
#endif
				None };

		Trace("Creating context");

		context = GlXCreateContextAttribsARB(display, frameBufferConfig, shared,
				True, contextAttribs);
	}
	// Sync to ensure any errors generated are processed.
	// XSync( display, False );
	// XSetErrorHandler( oldHandler );
	if (!context || !result) {
		Error("Context creation failed!.");
		NEX_THROW_FatalError(EXCEPT_DEVICE_CREATION_FAILED);
	}
	if (glXIsDirect(display, context))
		Trace("Direct GLX rendering context created!");
	else
		Trace("Indirect GLX rendering context created!");

	baseContext->SetCurrentTarget(static_cast<RenderWindowImpl*>(gw->GetImpl()));
}

nextar::RenderWindow* RenderContextImplGLX::CreateWindowImpl() {
	return NEX_NEW(WindowGLX(this));
}

Display* RenderContextImplGLX::OpenDisplay(int gpuIndex) {
	// assume that x-server index is same as gpu index
	if (gpuIndex < 0)
		gpuIndex = 0;
	String openId = ":0." + Convert::ToString((uint32) gpuIndex);
	display = XOpenDisplay(openId.c_str());
	if (!display) {
		Trace("Failed to open proper display!");
		display = XOpenDisplay(NULL);
	}
	return display;
}

bool RenderContextImplGLX::IsXRandrSupported() {
	int dummy;
	if (XQueryExtension(display, "RANDR", &dummy, &dummy, &dummy)
			&& XRRQueryVersion(display, &dummy, &dummy))
		return true;
	return false;
}

void RenderContextImplGLX::EnumVideoModes(VideoModeList& videoModes) {
	// check for xrandr support
	videoModes.clear();
	bool fallback = false;
	if ((xrandrSupported = IsXRandrSupported())) {
		XRRScreenSize* screenSize;
		int nsizes = 0;

		Trace("XRANDR is enabled");
		screenSize = XRRSizes(display, screenIndex, &nsizes);
		if (!nsizes)
			fallback = true;
		videoModes.reserve((size_t) nsizes);
		for (int i = 0; i < nsizes; ++i) {
			VideoMode vmode;
			vmode.width = uint16(screenSize[i].width);
			vmode.height = uint16(screenSize[i].height);

			int nrates;
			short* rates = XRRRates(display, screenIndex, i, &nrates);
			for (int j = 0; j < nrates; ++j) {
				vmode.refreshRate = uint16(rates[j]);
				videoModes.push_back(vmode);
			}
		}
		Window root = RootWindow(display, screenIndex);
		Rotation rotation;
		XRRScreenConfiguration *conf = XRRGetScreenInfo(display, root);
		int currsize = XRRConfigCurrentConfiguration(conf, &rotation);
		VideoMode current = VideoMode(screenSize[currsize].width,
				screenSize[currsize].height, XRRConfigCurrentRate(conf));
		std::sort(videoModes.begin(), videoModes.end());
		auto it = std::find(videoModes.begin(), videoModes.end(), current);
		if (it != videoModes.end())
			baseContext->SetOriginalVideoMode((uint32)(it - videoModes.begin()));
		else {
			videoModes.push_back(current);
			baseContext->SetOriginalVideoMode((uint32)videoModes.size() - 1);
		}
		XRRFreeScreenConfigInfo(conf);
	} /* check vid modes*/
	if (fallback || !videoModes.size()) {
		VideoMode vmode;
		vmode.width = DisplayWidth(display, screenIndex);
		vmode.height = DisplayHeight(display, screenIndex);
		vmode.refreshRate = 0;
		videoModes.push_back(vmode);
		baseContext->SetOriginalVideoMode((uint32)videoModes.size() - 1);
		Error("Viewmode set in fallback mode!");
	}
}

GLXFBConfig RenderContextImplGLX::GetConfig(int baseAttribs[], int maxAttribs[]) {

	int fbcount = 0;
	GLXFBConfig *fbconfigs = glXChooseFBConfig(display, screenIndex,
			baseAttribs, &fbcount);
	// sort configs
	int samples = 0;
	int chosen = -1;

	for (int i = 0; i < fbcount; ++i) {
		// check cavet
		int attrib_val;
		glXGetFBConfigAttrib(display, fbconfigs[i], GLX_CONFIG_CAVEAT,
				&attrib_val);

		if (attrib_val == GLX_NONE) {
			// check multisampling
			glXGetFBConfigAttrib(display, fbconfigs[i], GLX_SAMPLES,
					&attrib_val);
			if (samples < attrib_val || chosen < 0) {
				// check if we exceed expectations
				if (chosen >= 0) {
					bool exceeds = false;
					for (int k = 0; maxAttribs[k * 2] != None; ++k) {
						glXGetFBConfigAttrib(display, fbconfigs[i],
								maxAttribs[k * 2], &attrib_val);
						if (maxAttribs[k * 2 + 1] < attrib_val) {
							exceeds = true;
							break;
						}
					}
					if (!exceeds)
						chosen = i;
				} else
					chosen = i;
			}

		}
	}
	// simply we didnt enter the loop if fbcount <= 0
	if (chosen < 0) {
		Error("No suitable frame buffer config found!");
		NEX_THROW_GracefulError(EXCEPT_INVALID_ARGS);
	}

	GLXFBConfig chosenFb = fbconfigs[chosen];

	XFree(fbconfigs);

	return chosenFb;
}

void RenderContextImplGLX::SetCurrentWindow(RenderTarget* canvas) {
	if (canvas) {
		currentDrawable = static_cast<WindowGLX::Impl*>(canvas)->GetDrawable();
		glXMakeCurrent(display, currentDrawable, context);
	} else
		glXMakeCurrent(display, 0, 0);
}

void RenderContextImplGLX::SetVideoModeImpl(const VideoMode& mode) {
	if (xrandrSupported) {
		Window rootWnd = RootWindow(display, screenIndex);
		XRRScreenConfiguration *screenConfig = XRRGetScreenInfo(display,
				rootWnd);

		if (screenConfig) {

			int size = -1;
			int nsizes = 0;
			XRRScreenSize* screenSize = XRRConfigSizes(screenConfig, &nsizes);
			for (int i = 0; i < nsizes; ++i) {
				if (mode.width == uint16(screenSize[i].width)
						&& mode.height == uint16(screenSize[i].height)) {
					size = i;
					break;
				}
			}
			if (size < 0)
				// invalid mode
				return;
			Rotation current_rotation;
			XRRConfigCurrentConfiguration(screenConfig, &current_rotation);
			XRRSetScreenConfigAndRate(display, screenConfig, rootWnd, size,
					current_rotation, mode.refreshRate, CurrentTime);
			XRRFreeScreenConfigInfo(screenConfig);
			Trace(String("Video mode changed to:") + VideoMode::ToString(mode));
		}
	}
}

void RenderContextImplGLX::SwitchToFullScreen(Window win, bool toggle) {
	if (wmState != None) {
		// send the _NET_WM_STATE_FULLSCREEN msg
		XClientMessageEvent msg;
		msg.type = ClientMessage;
		msg.serial = 0;
		msg.send_event = True;
		msg.window = win;
		msg.message_type = wmState;
		msg.format = 32;
		msg.data.l[0] = toggle ? 1 : 0;
		msg.data.l[1] = toggle;
		msg.data.l[2] = 0;

		XSendEvent(display, RootWindow(display, screenIndex), False,
				SubstructureRedirectMask | SubstructureNotifyMask,
				(XEvent*) &msg);
	} else if (motif != None) {
		struct Hints {
			unsigned long flags;
			unsigned long functions;
			unsigned long decorations;
			long inputMode;
			unsigned long status;
		};
		Hints hints = { 0 };
		hints.flags = 2;
		hints.decorations = 0;
		XChangeProperty(display, win, motif, motif, 32,
		PropModeReplace, (unsigned char *) &hints, 5);
	} else
		Warn("Window manager is outdated, cannot switch to fullscreen");
}

void* RenderContext_Base_GL::GetExtension(const char* name) {
	size_t len = std::strlen(name);
	if (len > 3 && !std::strcmp((name + len - 3), "ARB"))
		return (void*) glXGetProcAddressARB((const GLubyte *) name);
	return (void*) glXGetProcAddress((const GLubyte *) name);
}

RenderContext_Base_GL::PlatformImpl* RenderContext_Base_GL::CreatePlatformImpl() {
		return NEX_NEW(RenderContextImplGLX(this));
}

}
