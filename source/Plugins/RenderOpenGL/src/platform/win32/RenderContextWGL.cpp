/*
 * RenderContextWGL.cpp
 *
 *  Created on: 11-Aug-2013
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <RenderDriverGL.h>
#include <RenderDriverWGL.h>
#include <RenderContextWGL.h>
#include <WindowWGL.h>

namespace RenderOpenGL {

RenderContextWGL::RenderContextWGL(RenderDriverWGL* _driver) :
		RenderContextGL(_driver), context(0)
		,WglCreateContextAttribsARB(0) {
}

RenderContextWGL::~RenderContextWGL() {
	UnreadyContext();
}

void RenderContextWGL::UnreadyContext() {
	if (context) {
		SetCurrentTarget(0);
		Trace("Destroying render context.");
		wglDeleteContext(context);
		context = 0;
	}
}

void RenderContextWGL::SetCreationParams(
		const RenderDriver::ContextCreationParams& ctxParams) {
	// lets check for glx extensions
	bool failed = false;
	contextCreationParams = ctxParams;
	int gpuIndex = ((RenderDriverWGL*) driver)->GetGpuIndex();
	std::memset(&pixelDescriptor, 0, sizeof(pixelDescriptor));
	/** We will use this pixel descriptor across all the created windows.
	 *  This will enable us using this context for all the HDC's
	 */
	pixelDescriptor.nSize = sizeof (pixelDescriptor);
	pixelDescriptor.nVersion = 1;
	pixelDescriptor.dwFlags = PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelDescriptor.cDepthBits = (BYTE) contextCreationParams.depthBits;
	pixelDescriptor.cStencilBits = (BYTE) contextCreationParams.stencilBits;
	pixelDescriptor.cColorBits = 24;
	pixelDescriptor.iLayerType = PFD_MAIN_PLANE;

	float fAttribs[] = { 0, 0 };

	DummyContext dc = CreateDummyContext();
	ReadyWglExtensions();
	EnumVideoModes();
	
	// choose fbconfig
	int iAttribs[] = { 
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_STENCIL_BITS_ARB, contextCreationParams.stencilBits,
		WGL_DEPTH_BITS_ARB, contextCreationParams.depthBits, 
		WGL_RED_BITS_ARB, 8, 
		WGL_GREEN_BITS_ARB, 8, 
		WGL_BLUE_BITS_ARB, 8, 
		0, 0,
		0, 0,
		0, 0,
		0, 0 
	};

	int attrib = 18;
	if(contextCreationParams.multiSamples) {
		iAttribs[attrib++] = WGL_SAMPLE_BUFFERS_ARB;
		iAttribs[attrib++] = GL_TRUE;
		iAttribs[attrib++] = WGL_SAMPLES_ARB;
		iAttribs[attrib++] = (int)contextCreationParams.multiSamples;
	}
	if(contextCreationParams.stereo) {
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

RenderContextWGL::DummyContext RenderContextWGL::CreateDummyContext() {
	WindowWGL::InitializeWindowClass();
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	HWND hWnd = CreateWindowEx (
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
			DestroyWindow (hWnd);
			hWnd = 0;
		}
		Error("Failed to create basic window!");
		NEX_THROW_FatalError(EXCEPT_FAILED_TO_CREATE_OBJECT);
	}

	SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
	int pixelFormat = ChoosePixelFormat (hDC, &pixelDescriptor);

	if (pixelFormat == 0 ||
		(SetPixelFormat (hDC, pixelFormat, 
		&pixelDescriptor) == FALSE) ||
		!(hRC = wglCreateContext (hDC))) {
		// Failed
		ReleaseDC (hWnd, hDC);
		DestroyWindow (hWnd);
		hWnd = 0;
		Error("Failed to get proper pixel format or create dummy context!");
		NEX_THROW_FatalError(EXCEPT_FAILED_TO_CREATE_OBJECT);
	}
	
	if (wglMakeCurrent (hDC, hRC) == FALSE) {
		// Failed
		wglDeleteContext (hRC);
		hRC = 0;
		ReleaseDC (hWnd, hDC);
		hDC = 0;
		DestroyWindow (hWnd);
		hWnd = 0;
		Error("Failed to set context!");
		NEX_THROW_FatalError(EXCEPT_FAILED_TO_CREATE_OBJECT);
	}

	DummyContext r = { hWnd, hDC, hRC };
	return r;
}

void RenderContextWGL::DestroyDummyContext(const RenderContextWGL::DummyContext& dc) {
	wglMakeCurrent (NULL, NULL);
	// Failed
	wglDeleteContext (dc.hRC);
	ReleaseDC (dc.hWnd, dc.hDC);
	DestroyWindow (dc.hWnd);
}

void RenderContextWGL::ReadyWglExtensions() {
	const char *supported = NULL;
 
	// Try To Use wglGetExtensionStringARB On Current DC, If Possible
	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");
 
	if (wglGetExtString)
		supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());
 
	// If That Failed, Try Standard Opengl Extensions String
	if (supported == NULL)
		supported = (char*)glGetString(GL_EXTENSIONS);

	if (IsSupported("WGL_ARB_create_context", supported))
		WglCreateContextAttribsARB =
				(PFNWGLCREATECONTEXTATTRIBSARBPROC) 
				RenderDriverGL::GetExtension("wglCreateContextAttribsARB");

	if (IsSupported("WGL_ARB_pixel_format", supported))
		WglChoosePixelFormatARB =
		(PFNWGLCHOOSEPIXELFORMATARBPROC)
		RenderDriverGL::GetExtension("wglChoosePixelFormatARB");
}

void RenderContextWGL::ReadyContext(RenderWindow* gw) {
	// create context based of fbConfig and set as current
	// do we have a shared context?
	WindowWGL* wglWnd = static_cast<WindowWGL*>(gw);
	HGLRC shared = 0;
	if (contextCreationParams.sharedContextIndex >= 0) {
		RenderContextPtr ptr = driver->AsyncGetContext(
				contextCreationParams.sharedContextIndex);
		if (ptr) {
			shared = (static_cast<RenderContextWGL*>(ptr.GetPtr()))->context;
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

	SetCurrentTarget(static_cast<RenderWindowImpl*>(gw->GetImpl()));
}

nextar::RenderWindow* RenderContextWGL::CreateWindowImpl() {
	return NEX_NEW(WindowWGL(this));
}


void RenderContextWGL::EnumVideoModes() {
	videoModes.clear();
	DEVMODE dmi;

	DWORD devNum = 0;
	DWORD modeNum = 0;
	DISPLAY_DEVICE ddi;


	ZeroMemory(&ddi, sizeof(ddi));
	ddi.cb = sizeof (ddi);
	ZeroMemory(&dmi, sizeof(dmi));
	dmi.dmSize = sizeof (dmi);

	while (EnumDisplayDevices(NULL, devNum++, &ddi, 0)) {
		while (EnumDisplaySettings(ddi.DeviceName, modeNum++, &dmi)) {
			VideoMode vm((uint16) dmi.dmPelsWidth,
				(uint16) dmi.dmPelsHeight,
				(uint16) dmi.dmDisplayFrequency);
			if (vm.IsValid() && 
				std::find(videoModes.begin(), videoModes.end(), vm) == videoModes.end())
				videoModes.push_back(vm);
			ZeroMemory(&dmi, sizeof(dmi));
			dmi.dmSize = sizeof (dmi);
		}
		ZeroMemory(&ddi, sizeof(ddi));
		ddi.cb = sizeof (ddi);
		modeNum = 0;
	}

	std::sort(videoModes.begin(), videoModes.end());
	// get the current mode
	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmi)) {
		VideoMode vm((uint16) dmi.dmPelsWidth,
			(uint16) dmi.dmPelsHeight,
			(uint16) dmi.dmDisplayFrequency);
		auto it = std::find(videoModes.begin(), videoModes.end(), vm);
		if (it != videoModes.end())
			currentVideoMode = originalVideoMode = it-videoModes.begin();
		else {
			videoModes.push_back(vm);
			currentVideoMode = originalVideoMode = videoModes.size()-1;
		}
	} else {
		Error("Failed to enumerate current mode");
		currentVideoMode = originalVideoMode = 0;
	}
}

int RenderContextWGL::GetFormat(const DummyContext& dc, float fAttribs[], int iAttribs[]) {
	
	int pixelFormat = 0;
	UINT numFormats = 0;
	
	if ( !WglChoosePixelFormatARB ||
		!WglChoosePixelFormatARB( dc.hDC, 
		iAttribs, fAttribs, 1, &pixelFormat, &numFormats) ||
		numFormats == 0) {
		return ChoosePixelFormat (dc.hDC, &pixelDescriptor);
	} 

	return pixelFormat;
}

void RenderContextWGL::SetCurrentWindow(RenderTarget* canvas) {
	if (canvas) {
		WindowWGL::Impl* impl = static_cast<WindowWGL::Impl*>(canvas);
		if (impl->GetWindowDC() != currentDC) {
			wglMakeCurrent(currentDC = impl->GetWindowDC(), context);
		}
	} else
		wglMakeCurrent(0, 0);
	GL_CHECK();
}

void RenderContextWGL::SetVideoModeImpl(const VideoMode& mode) {
	DEVMODE dm = {0};
	dm.dmPelsWidth = mode.width;
	dm.dmPelsHeight = mode.height;
	dm.dmDisplayFrequency = mode.refreshRate;
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
	if(mode.refreshRate)
		dm.dmFields |= DM_DISPLAYFREQUENCY;
	
	dm.dmSize = sizeof (DEVMODE);
	if (DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettings(&dm, CDS_TEST)) {
		ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	} else {
		Error("Error setting video mode.");
		NEX_THROW_GracefulError(EXCEPT_INVALID_ARGS);
	}
}

}

