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
#include <ContextObject.h>

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

	// @remarks
	// Sub-classed by @RenderTexture, @MultiRenderTarget and @RenderBuffer
	class _NexEngineAPI RenderTarget : public Referenced<RenderTarget, AllocGraphics> {
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
		/* Capture render target from a single context */
		virtual void Capture(RenderContext* rc, PixelBox& image, FrameBuffer) = 0;
		// virtual void Reset(RenderContext* rc, Size size, PixelFormat format) = 0;
		// todo This call should be replaced by the view->Present, may be a list of views
		// should be maintaned instead of targets.
		virtual void Present(RenderContext* rc) = 0;

	protected:

		const RenderTargetType targetType;
		Viewport* viewport;
	};

	class RenderTargetView : public ContextObject::View {
	public:
		virtual void Capture(RenderContext* rc, PixelBox& image, FrameBuffer) = 0;
		virtual void Present(RenderContext* rc) = 0;
	};

} /* namespace nextar */
#endif /* RENDERTARGET_H_ */
