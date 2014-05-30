/*
 * RenderBuffer.h
 *
 *  Created on: 30-May-2014
 *      Author: obhi
 */

#ifndef RENDERBUFFER_H_
#define RENDERBUFFER_H_

#include <ContextObject.h>
#include <RenderTarget.h>

namespace nextar {

	class _NexEngineAPI RenderBuffer: public ContextObject,
		public RenderTarget {
	public:

		enum {
			MSG_RB_CREATE = 1 << 1,
		};

		struct CreateParams {
			PixelFormat format;
			uint16 width;
			uint16 height;
		};

		RenderBuffer();
		virtual ~RenderBuffer();

		virtual Size GetDimensions() const;
		virtual PixelFormat GetPixelFormat() const;

		virtual void Create(PixelFormat format, uint32 width, uint32 height);
		virtual void Capture(RenderContext* rc, PixelBox& image, FrameBuffer);
		virtual void Present(RenderContext* rc);
z
	protected:
		uint16 width, height;
		PixelFormat format;
	};

} /* namespace nextar */

#endif /* RENDERBUFFER_H_ */
