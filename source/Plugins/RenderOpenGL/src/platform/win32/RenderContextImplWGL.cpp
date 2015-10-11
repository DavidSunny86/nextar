/*
 * RenderContextImplWGL.cpp
 *
 *  Created on: 11-Aug-2013
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <RenderDriverGL.h>
#include <RenderContextImplWGL.h>
#include <WindowWGL.h>

namespace RenderOpenGL {

RenderContextImplWGL::RenderContextImplWGL(RenderContext_Base_GL* baseContext) :
RenderContext_Base_GL::PlatformImpl(baseContext), context(0)
, WglCreateContextAttribsARB(0) {
}

RenderContextImplWGL::~RenderContextImplWGL() {
}

void RenderContextImplWGL::PostCloseImpl() {
	UnreadyContext();
}

void RenderContextImplWGL::UnreadyContext() {
	if (context) {
		baseContext->SetCurrentTarget(0);
		Trace("Destroying render context.");
		wglDeleteContext(context);
		context = 0;
	}
}

void RenderContextImplWGL::SetCreationParams(
	const RenderDriver::ContextCreationParams& ctxParams) {
	// lets check for glx extensions
	bool failed = false;
	baseContext->SetContextParams(ctxParams);
	int gpuIndex = ((RenderDriverGL*)baseContext->GetDriver())->GetGpuIndex();
	std::memset(&pixelDescriptor, 0, sizeof(pixelDescriptor));
	/** We will use this pixel descriptor across all the created windows.
	 *  This will enable us using this context for all the HDC's
	 */
	pixelDescriptor.nSize = sizeof(pixelDescriptor);
	pixelDescriptor.nVersion = 1;
	pixelDescriptor.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelDescriptor.cDepthBits = (BYTE)ctxParams.depthBits;
	pixelDescriptor.cStencilBits = (BYTE)ctxParams.stencilBits;
	pixelDescriptor.cColorBits = 24;
	pixelDescriptor.iLayerType = PFD_MAIN_PLANE;

	float fAttribs[] = { 0, 0 };

	DummyContext dc = CreateDummyContext();
	ReadyWglExtensions();
	VideoModeList modes;
	EnumVideoModes(modes);
	baseContext->SetVideoModes(std::move(modes));

	// choose fbconfig
	int iAttribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_STENCIL_BITS_ARB, ctxParams.stencilBits,
		WGL_DEPTH_BITS_ARB, ctxParams.depthBits,
		WGL_RED_BITS_ARB, 8,
		WGL_GREEN_BITS_ARB, 8,
		WGL_BLUE_BITS_ARB, 8,
		0, 0,
		0, 0,
		0, 0,
		0, 0
	};

	int attrib = 18;
	if (ctxParams.multiSamples) {
		iAttribs[attrib++] = WGL_SAMPLE_BUFFERS_ARB;
		iAttribs[attrib++] = GL_TRUE;
		iAttribs[attrib++] = WGL_SAMPLES_ARB;
		iAttribs[attrib++] = (int)ctxParams.multiSamples;
	}
	if (ctxParams.stereo) {
		iAttribs[attrib++] = WGL_STEREO_ARB;
		iAttribs[attrib++] = GL_TRUE;
	}

	pixelFormat = GetFormat(dc, fAttribs, iAttribs);
	DescribePixelFormat(dc.hDC, pixelFormat,
						sizeof(PIXELFORMATDESCRIPTOR), &pixelDescriptor);
	DestroyDummyContext(dc);
	// create context

	if (failed)
		NEX_THROW_FatalError(EXCEPT_DEVICE_CREATION_FAILED);
}

RenderContextImplWGL::DummyContext RenderContextImplWGL::CreateDummyContext() {
	WindowWGL::InitializeWindowClass();
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	HWND hWnd = CreateWindowEx(
		windowExtendedStyle,					// Extended Style
		(LPCSTR)WindowWGL::GetWindowClass(),	// Class Name
		"DummyWindow",					// Window Title
		windowStyle,							// Window Style
		0, 0,								// Window X,Y Position
		32,	// Window Width
		32,	// Window Height
		HWND_DESKTOP,						// Desktop Is Window's Parent
		0,									// No Menu
		hInstance, // Pass The Window Instance
		0);
	HDC hDC;
	HGLRC hRC;
	if (!hWnd || !(hDC = GetDC(hWnd))) {
		if (hWnd) {
			DestroyWindow(hWnd);
			hWnd = 0;
		}
		Error("Failed to create basic window!");
		NEX_THROW_FatalError(EXCEPT_FAILED_TO_CREATE_OBJECT);
	}

	SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
	int pixelFormat = ChoosePixelFormat(hDC, &pixelDescriptor);

	if (pixelFormat == 0 ||
		(SetPixelFormat(hDC, pixelFormat,
		&pixelDescriptor) == FALSE) ||
		!(hRC = wglCreateContext(hDC))) {
		// Failed
		ReleaseDC(hWnd, hDC);
		DestroyWindow(hWnd);
		hWnd = 0;
		Error("Failed to get proper pixel format or create dummy context!");
		NEX_THROW_FatalError(EXCEPT_FAILED_TO_CREATE_OBJECT);
	}

	if (wglMakeCurrent(hDC, hRC) == FALSE) {
		// Failed
		wglDeleteContext(hRC);
		hRC = 0;
		ReleaseDC(hWnd, hDC);
		hDC = 0;
		DestroyWindow(hWnd);
		hWnd = 0;
		Error("Failed to set context!");
		NEX_THROW_FatalError(EXCEPT_FAILED_TO_CREATE_OBJECT);
	}

	DummyContext r = { hWnd, hDC, hRC };
	return r;
}

void RenderContextImplWGL::DestroyDummyContext(const RenderContextImplWGL::DummyContext& dc) {
	wglMakeCurrent(NULL, NULL);
	// Failed
	if (!wglDeleteContext(dc.hRC))
		Error("Failed to delete dummy context!");
	ReleaseDC(dc.hWnd, dc.hDC);
	DestroyWindow(dc.hWnd);
}

void RenderContextImplWGL::ReadyWglExtensions() {
	const char *supported = NULL;

	// Try To Use wglGetExtensionStringARB On Current DC, If Possible
	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

	if (wglGetExtString)
		supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());

	// If That Failed, Try Standard Opengl Extensions String
	if (supported == NULL)
		supported = (char*)glGetString(GL_EXTENSIONS);

	if (ExtensionHelper::IsSupported("WGL_ARB_create_context", supported))
		WglCreateContextAttribsARB =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)
		RenderContext_Base_GL::GetExtension("wglCreateContextAttribsARB");

	if (ExtensionHelper::IsSupported("WGL_ARB_pixel_format", supported))
		WglChoosePixelFormatARB =
		(PFNWGLCHOOSEPIXELFORMATARBPROC)
		RenderContext_Base_GL::GetExtension("wglChoosePixelFormatARB");
}

void RenderContextImplWGL::ReadyContext(RenderWindow* gw) {
	// create context based of fbConfig and set as current
	// do we have a shared context?
	WindowWGL* wglWnd = static_cast<WindowWGL*>(gw);
	HGLRC shared = 0;
	const RenderDriver::ContextCreationParams& contextCreationParams = baseContext->GetContextParams();
	RenderDriverGL* driver = (RenderDriverGL*)baseContext->GetDriver();
	if (contextCreationParams.sharedContextIndex >= 0) {
		RenderContextPtr ptr = driver->AsyncGetContext(
			contextCreationParams.sharedContextIndex);
		if (ptr) {
			RenderContext_Base_GL* b = static_cast<RenderContext_Base_GL*>(ptr.GetPtr());
			shared = (static_cast<RenderContextImplWGL*>(b->GetImpl()))->context;
		}
	}

	bool result = true;
	// int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler([&result](Display*, XErrorEvent*) { result = false; return 0; } );
	if (!WglCreateContextAttribsARB) {
		Trace("Creating old style context");
		context = wglCreateContext(wglWnd->GetWindowDC());
	} else {
		int contextAttribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB,
			contextCreationParams.reqOpenGLVersionMajor,
			WGL_CONTEXT_MINOR_VERSION_ARB,
			contextCreationParams.reqOpenGLVersionMinor,
#ifdef NEX_DEBUG
			WGL_CONTEXT_FLAGS_ARB,
			WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
			0, 0
		};

		Trace("Creating context");
		context = WglCreateContextAttribsARB(
			wglWnd->GetWindowDC(), shared, contextAttribs);
	}
	// Sync to ensure any errors generated are processed.
	// XSync( display, False );
	// XSetErrorHandler( oldHandler );
	if (!context || !result) {
		Error("Context creation failed!.");
		NEX_THROW_FatalError(EXCEPT_DEVICE_CREATION_FAILED);
	}

	baseContext->SetCurrentTarget(static_cast<RenderWindowImpl*>(gw->GetImpl()));
	GL_CHECK();
}

nextar::RenderWindow* RenderContextImplWGL::CreateWindowImpl() {
	return NEX_NEW(WindowWGL(this));
}


void RenderContextImplWGL::EnumVideoModes(VideoModeList& videoModes) {
	videoModes.clear();
	DEVMODE dmi;

	DWORD devNum = 0;
	DWORD modeNum = 0;
	DISPLAY_DEVICE ddi;


	ZeroMemory(&ddi, sizeof(ddi));
	ddi.cb = sizeof(ddi);
	ZeroMemory(&dmi, sizeof(dmi));
	dmi.dmSize = sizeof(dmi);

	while (EnumDisplayDevices(NULL, devNum++, &ddi, 0)) {
		while (EnumDisplaySettings(ddi.DeviceName, modeNum++, &dmi)) {
			VideoMode vm((uint16)dmi.dmPelsWidth,
						 (uint16)dmi.dmPelsHeight,
						 (uint16)dmi.dmDisplayFrequency);
			if (vm.IsValid() &&
				std::find(videoModes.begin(), videoModes.end(), vm) == videoModes.end())
				videoModes.push_back(vm);
			ZeroMemory(&dmi, sizeof(dmi));
			dmi.dmSize = sizeof(dmi);
		}
		ZeroMemory(&ddi, sizeof(ddi));
		ddi.cb = sizeof(ddi);
		modeNum = 0;
	}

	std::sort(videoModes.begin(), videoModes.end());
	// get the current mode
	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmi)) {
		VideoMode vm((uint16)dmi.dmPelsWidth,
					 (uint16)dmi.dmPelsHeight,
					 (uint16)dmi.dmDisplayFrequency);
		auto it = std::find(videoModes.begin(), videoModes.end(), vm);
		if (it != videoModes.end())
			baseContext->SetOriginalVideoMode((uint32)(it - videoModes.begin()));
		else {
			videoModes.push_back(vm);
			baseContext->SetOriginalVideoMode((uint32)videoModes.size() - 1);
		}
	} else {
		Error("Failed to enumerate current mode");
		baseContext->SetOriginalVideoMode(0);
	}
}

int RenderContextImplWGL::GetFormat(const DummyContext& dc, float fAttribs[], int iAttribs[]) {

	int pixelFormat = 0;
	UINT numFormats = 0;

	if (!WglChoosePixelFormatARB ||
		!WglChoosePixelFormatARB(dc.hDC,
		iAttribs, fAttribs, 1, &pixelFormat, &numFormats) ||
		numFormats == 0) {
		return ChoosePixelFormat(dc.hDC, &pixelDescriptor);
	}

	return pixelFormat;
}

void RenderContextImplWGL::SetCurrentWindow(RenderTarget* canvas) {
	if (canvas) {
		WindowWGL::Impl* impl = static_cast<WindowWGL::Impl*>(canvas);
		if (impl->GetWindowDC() != currentDC) {
			if (!wglMakeCurrent(currentDC = impl->GetWindowDC(), context))
				Error("Failed to set context!");
		}
	} else
		wglMakeCurrent(0, 0);
}

void RenderContextImplWGL::SetVideoModeImpl(const VideoMode& mode) {
	DEVMODE dm = { 0 };
	dm.dmPelsWidth = mode.width;
	dm.dmPelsHeight = mode.height;
	dm.dmDisplayFrequency = mode.refreshRate;
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
	if (mode.refreshRate)
		dm.dmFields |= DM_DISPLAYFREQUENCY;

	dm.dmSize = sizeof(DEVMODE);
	if (DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettings(&dm, CDS_TEST)) {
		ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	} else {
		Error("Error setting video mode.");
		NEX_THROW_GracefulError(EXCEPT_INVALID_ARGS);
	}
}

RenderContext_Base_GL::PlatformImpl* RenderContext_Base_GL::CreatePlatformImpl() {
	return NEX_NEW(RenderContextImplWGL(this));
}

void* RenderContext_Base_GL::GetExtension(const char* name) {
	size_t len = std::strlen(name);
	if (len > 3 && !std::strcmp((name + len - 3), "ARB"))
		return (void*)wglGetProcAddress((LPCSTR)name);
	return (void*)wglGetProcAddress((LPCSTR)name);
}

}
