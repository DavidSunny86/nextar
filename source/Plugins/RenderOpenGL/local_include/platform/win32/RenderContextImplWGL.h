#ifndef RENDER_CONTEXT_IMPL_WGL_H_
#define RENDER_CONTEXT_IMPL_WGL_H_

#include <RenderOpenGL.h>
#include <RenderContext_Base_GL.h>
#include <WindowWGL.h>

namespace RenderOpenGL {

class RenderDriverWGL;

class RenderContextImplWGL: public RenderContext_Base_GL::PlatformImpl {

	NEX_LOG_HELPER(RenderContextWGL);

public:
	RenderContextImplWGL(RenderContext_Base_GL* pParent);
	virtual ~RenderContextImplWGL();

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
	inline uint32 GetVideoModeIndex(const VideoMode& m) {
		return baseContext->GetVideoModeIndex(m);
	}

	inline void SetVideoMode(uint32 m) {
		baseContext->SetVideoMode(m);
	}

	inline const VideoMode& GetCurrentMode() const {
		return baseContext->GetCurrentMode();
	}

	inline void SetCurrentTarget(RenderTarget* t) {
		baseContext->SetCurrentTarget(t);
	}

	inline void DestroyedRenderWindow(RenderWindow* w) {
		baseContext->DestroyedRenderWindow(w);
	}

	
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
	void EnumVideoModes(VideoModeList& videModes);
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

#endif // RENDER_CONTEXT_IMPL_WGL_H_
