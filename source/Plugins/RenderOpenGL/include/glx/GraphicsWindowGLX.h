/*
 * GraphicsWindowGLX.h
 *
 *  Created on: 11-Aug-2013
 *      Author: obhi
 */

#ifndef GRAPHICSWINDOWGLX_H_
#define GRAPHICSWINDOWGLX_H_

#include "RenderOpenGL.h"
#include "GraphicsWindow.h"
#include "glx/CanvasGLX.h"

namespace RenderOpenGL {

	class RenderContextGLX;

	class GraphicsWindowGLX: public nextar::RenderWindowX {

		NEX_LOG_HELPER(GraphicsWindowGLX);

		class WindowCanvas: public CanvasGLX, public nextar::AllocGraphics {
		public:
			WindowCanvas(GraphicsWindowGLX* parent, const nextar::String& name);

		private:
			GraphicsWindowGLX* parent;
		};

	public:

		GraphicsWindowGLX(RenderContextGLX* binding);
		virtual ~GraphicsWindowGLX();

	private:
	public:

		RenderContextGLX* GetContext() {
			return context;
		}

		_NexInline Window GetDrawable() {
			return window;
		}

		virtual nextar::RenderTarget* GetCanvas() {
			return &canvas;
		}

		virtual void CreateImpl(nextar::uint32 width, nextar::uint32 height, bool fullscreen,
				const nextar::NameValueMap* params);
		virtual void SetToFullScreen(bool fullscreen);
		virtual void DestroyImpl();

	protected:

		uint16 width;
		uint16 height;
		uint16 left;
		uint16 top;


		Colormap cmap;
		WindowCanvas canvas;
		RenderContextGLX* context;
	};

}

#endif /* GRAPHICSWINDOWGLX_H_ */
