/*
 * RenderTarget.h
 *
 *  Created on: 24-Nov-2013
 *      Author: obhi
 */

#ifndef RENDERTARGET_H_
#define RENDERTARGET_H_

#include <TextureBase.h>
#include <RefPtr.h>
#include <Viewport.h>

namespace nextar {

	enum class FrameBuffer {
		FRONT_LEFT,
		FRONT_RIGHT,
		BACK_LEFT,
		BACK_RIGHT,
		DEPTH,
		FRONT = FRONT_LEFT,
		BACK = BACK_LEFT
	};

	enum class RenderTargetType : uint16 {
		RENDER_TEXTURE,
		RENDER_WINDOW,
		MULTI_RENDER_TARGET,
	};

	class RenderTarget : public Referenced<RenderTarget, AllocGraphics> {
	public:

		RenderTarget(RenderTargetType type);
		virtual ~RenderTarget();

		RenderTargetType GetRenderTargetType() const {
			return (RenderTargetType)targetType;
		}
		/**
		 * Get the only viewport attached
		 */
		virtual Viewport::Iterator GetViewports();
		/**
		 * @brief Add a viewport. Overlapping viewports are allowed.
		 * @param priority This defines when the viewport is updated amongst other
		 viewports in this canvas. Lesser the number, the later
		 it is updated.
		 **/
		virtual Viewport* CreateViewport(Camera*, float x = 0.0f,
				float y = 0.0f, float width = 1.0f, float height = 1.0f,
				int32 priority = 0);
		/* pixel format */
		virtual PixelFormat GetPixelFormat() const = 0;
		/* Dimensions */
		virtual Size GetDimensions() const = 0;
		/* Capture render target */
		virtual void Capture(RenderContext* rc, PixelBox& image, FrameBuffer) = 0;
		virtual void Reset(RenderContext* rc, Size size, PixelFormat format) = 0;
		virtual void Present(RenderContext* rc) = 0;

	protected:

		const RenderTargetType targetType;
		Viewport* viewport;
	};

} /* namespace nextar */
#endif /* RENDERTARGET_H_ */
