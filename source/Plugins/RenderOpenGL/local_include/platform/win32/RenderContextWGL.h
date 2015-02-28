#pragma once

#include <RenderOpenGL.h>
#include <RenderContextGL.h>
#include <WindowWGL.h>

namespace RenderOpenGL {

class RenderDriverWGL;

class RenderContextWGL: public RenderContextGL {

	NEX_LOG_HELPER(RenderContextWGL)
	;

public:
	RenderContextWGL(RenderDriverWGL* driver);
	virtual ~RenderContextWGL();

	int GetPixelFormat() const {
		return pixelFormat;
	}

	const PIXELFORMATDESCRIPTOR* GetPixelDescriptor() const {
		return &pixelDescriptor;
	}

	inline bool IsCurrentDC(HDC hDC) {
		return (currentDC == hDC) != 0;
	}

	inline void Present(HDC hDC) {
#ifdef NEX_DEBUG
		BOOL ret =
#endif
			SwapBuffers(hDC);
			//wglSwapLayerBuffers(hDC,	WGL_SWAP_MAIN_PLANE);

#ifdef NEX_DEBUG
		if (!ret) {
			uint32 faliure;
			if (faliure = GetLastError()) {
				Warn("Failed on swap: " + Convert::ToString(faliure));
			}
		}
#endif
	}

	/* Implementations */
	virtual void SetVideoModeImpl(const VideoMode& videoMode);
	virtual void SetCurrentWindow(RenderTarget* rt);
	virtual void PostCloseImpl();
	/* Helpers */

protected:

	virtual void SetCreationParams(const RenderDriver::ContextCreationParams& ctxParams);
	virtual nextar::RenderWindow* CreateWindowImpl();
	virtual void ReadyContext(RenderWindow*);

private:

	struct DummyContext {
		HWND hWnd;
		HDC hDC;
		HGLRC hRC;
	};

	int GetFormat(const DummyContext&, float fAttribs[], int iAttribs[]);
	void UnreadyContext();
	void EnumVideoModes();
	void ReadyWglExtensions();
	DummyContext CreateDummyContext();
	void DestroyDummyContext(const DummyContext&);

protected:

	typedef void (*GLfunction)();
	/* GLX Extensions */
	PFNWGLCHOOSEPIXELFORMATARBPROC WglChoosePixelFormatARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC WglCreateContextAttribsARB;
	
	HDC currentDC;
	HGLRC context;
	int pixelFormat;
	PIXELFORMATDESCRIPTOR pixelDescriptor;
};


}
