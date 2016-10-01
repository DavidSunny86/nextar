#pragma once

#include <RenderOpenGL.h>
#include <WindowGLX.h>

namespace RenderOpenGL {

class RenderDriverGLX;

class RenderContextImplGLX: public RenderContext_Base_GL::PlatformImpl {

	NEX_LOG_HELPER(RenderContextImplGLX)
	;

public:
	RenderContextImplGLX(RenderContext_Base_GL* driver);
	virtual ~RenderContextImplGLX();

	inline Display* GetDisplay() {
		return display;
	}

	inline int GetScreenIndex() {
		return screenIndex;
	}

	inline GLXFBConfig GetFrameBufferConfig() {
		return frameBufferConfig;
	}

	inline bool IsCurrentDrawable(GLXDrawable currentDrawable) {
		return currentDrawable == this->currentDrawable;
	}

	/* Implementations */
	virtual void SetVideoModeImpl(const VideoMode& videoMode);
	virtual void SetCurrentWindow(RenderTarget* rt);
	virtual void PostCloseImpl();

	/* Helpers */
	void SwitchToFullScreen(Window drawable, bool value);

	inline void Present(WindowGLX::Impl* window) {
		glXSwapBuffers(display, window->GetDrawable());
		GL_CHECK();
	}

	protected:

		virtual void SetCreationParams(const RenderDriver::ContextCreationParams& ctxParams);
		virtual nextar::RenderWindow* CreateWindowImpl();
		virtual void ReadyContext(RenderWindow*);

	private:

		enum AttribComparison : uint8 {
			eBitCompare,
			eEquate,
			eLessThan,
			eLessEqual,
			eMoreThan,
			eMoreEqual,
		};

		GLXFBConfig GetConfig(int baseAttribs[], int maxAttribs[], const AttribComparison c[]);
		void UnreadyContext();
		void EnumVideoModes(VideoModeList&);
		bool IsXRandrSupported();
		Display* OpenDisplay(int gpuIndex);
		void CloseDisplay();
		void ReadyGlxExtensions();

	protected:

		typedef void (*GLfunction)();
		typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARB)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
		/* GLX Extensions */
		PFNGLXCREATECONTEXTATTRIBSARB GlXCreateContextAttribsARB;

		bool xrandrSupported;
		int screenIndex;

		Atom motif;
		Atom fullScreen;
		Atom wmState;

		Display* display;

		GLXDrawable currentDrawable;
		GLXContext context;
		GLXFBConfig frameBufferConfig;
	};
}
