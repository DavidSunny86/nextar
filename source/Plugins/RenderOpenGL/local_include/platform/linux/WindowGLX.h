/*
 * GraphicsWindowGLX.h
 *
 *  Created on: 11-Aug-2013
 *      Author: obhi
 */

#ifndef GRAPHICSWINDOWGLX_H_
#define GRAPHICSWINDOWGLX_H_

#include <RenderOpenGL.h>
#include <XWindow.h>
#include <RenderWindowImpl.h>

namespace RenderOpenGL {

class RenderContextGLX;

class WindowGLX: public nextar::XWindow {

	NEX_LOG_HELPER(WindowGLX)
	;

	class Impl: public RenderWindowImpl {
		NEX_LOG_HELPER(WindowGLX::Impl)
		;
	public:
		enum {
			PRE_FRAME_CAPTURE = (uint32) RenderConstants::MAX_FRAME_PRE_CAPTURE
		};

		Impl(WindowGLX* parent);

		// RenderWindow::Impl
		virtual void SetToFullScreen(bool fullscreen) override;
		virtual void Destroy() override;
		virtual void Create(uint32 width, uint32 height, bool fullscreen,
				const NameValueMap* params) override;
		virtual void ApplyChangedAttributes() override;
		virtual void FocusChanged() override;

		// RenderTarget
		virtual PixelFormat GetPixelFormat() const override;
		virtual void Capture(RenderContext* rc, PixelBox& image, FrameBuffer);
		virtual void Reset(RenderContext* rc, Size size, PixelFormat format);
		virtual void Present(RenderContext* rc);

		// Helpers
		inline GLXDrawable GetDrawable() const {
			return drawable;
		}

	private:
		GLXDrawable drawable;
		Point position;
		Colormap cmap;
		// for async capture
		GLuint pbo[PRE_FRAME_CAPTURE];
		WindowGLX* parent;
	};

public:

	WindowGLX(RenderContextGLX* binding);
	virtual ~WindowGLX();

	RenderContextGLX* GetContext() {
		return context;
	}

	inline void SetWindow(Window window) {
		this->window = window;
	}

	inline void SetDestroyMsg(Atom& m) {
		msgDestroy = m;
	}

	inline void SetDisplay(Display* dpy) {
		display = dpy;
	}

	inline void SetWindowTitle(const String& title) {
		windowTitle = std::move(title);
	}

protected:

	RenderContextGLX* context;
};

}

#endif /* GRAPHICSWINDOWGLX_H_ */
