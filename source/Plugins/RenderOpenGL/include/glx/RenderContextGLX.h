
#pragma once

#include "RenderOpenGL.h"
#include "RenderContextGL.h"

namespace RenderOpenGL {

	class RenderDriverGLX;

	class RenderContextGLX : public RenderContextGL {

		NEX_LOG_HELPER(RenderContextGLX);

	public:
		RenderContextGLX(RenderDriverGLX* driver);
		virtual ~RenderContextGLX();

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
		virtual void SetCurrentCanvas(RenderTarget*);
		virtual void SetVideoModeImpl(const VideoMode& videoMode);

		/* Helpers */
		void SwitchToFullScreen(Window drawable, bool value);

	protected:

		virtual void SetCreationParams(const RenderDriver::ContextCreationParams& ctxParams);
		virtual nextar::RenderWindow* CreateWindowImpl();
		virtual void ReadyContext(RenderWindow*);

	private:

		GLXFBConfig GetConfig(int baseAttribs[], int maxAttribs[]);
		void UnreadyContext();
		void EnumVideoModes();
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
		GLXContext  context;
		GLXFBConfig frameBufferConfig;

	};
}
