/*
 * GraphicsWindowGLX.h
 *
 *  Created on: 11-Aug-2013
 *      Author: obhi
 */

#ifndef GRAPHICSWINDOWWGL_H_
#define GRAPHICSWINDOWWGL_H_

#include <RenderOpenGL.h>
#include <Win32Window.h>
#include <RenderWindowImpl.h>

namespace RenderOpenGL {

class RenderContextImplWGL;

class WindowWGL: public nextar::Win32Window {

	NEX_LOG_HELPER(WindowWGL);
public:
	class Impl: public RenderWindowImpl {
		NEX_LOG_HELPER(WindowWGL::Impl)
		;
	public:
		enum {
			PRE_FRAME_CAPTURE = (uint32) RenderConstants::MAX_FRAME_PRE_CAPTURE
		};

		Impl(WindowWGL* parent);

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
		virtual void Present(RenderContext* rc);

		inline HDC GetWindowDC() {
			return hDC;
		}

		virtual void ResizeImpl(Size size);
		// Helpers
	private:
		
		HDC hDC;
		Size fsSwitchSize;
		Point position;
		// for async capture
		GLuint pbo[PRE_FRAME_CAPTURE];
		WindowWGL* parent;
	};

public:

	WindowWGL(RenderContextImplWGL* binding);
	virtual ~WindowWGL();

	RenderContextImplWGL* GetContext() {
		return context;
	}

	inline void SetWindowTitle(const String& title) {
		windowTitle = std::move(title);
	}

	static inline ATOM GetWindowClass() {
		return wndClass;
	}

	static void InitializeWindowClass();
protected:
	
	HGLRC hGL;
	RenderContextImplWGL* context;

	static ATOM wndClass;
};

}

#endif /* GRAPHICSWINDOWWGL_H_ */
