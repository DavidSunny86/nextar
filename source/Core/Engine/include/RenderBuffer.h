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

class _NexEngineAPI RenderBuffer: public ContextObject, public RenderTarget {
public:

	enum {
		MSG_RB_CREATE = 1 << 1,
		MSG_RB_READ = 1 << 2,
		MSG_RB_RESIZE = 1 << 3,
	};

	struct CreateParams {
		PixelFormat format;
		Size dimensions;
		uint16 samples;
	};

	RenderBuffer();
	virtual ~RenderBuffer();

	virtual Size GetDimensions() const;
	virtual PixelFormat GetPixelFormat() const;

	inline void Create(PixelFormat format, uint32 width, uint32 height,
			uint32 samples = 0) {
		CreateParams params;
		params.format = format;
		params.dimensions.dx = width;
		params.dimensions.dy = height;
		params.samples = samples;
		Create(params);
	}

	virtual void Create(const CreateParams& params);
	virtual void Capture(RenderContext* rc, PixelBox& image, FrameBuffer);

	virtual void ResizeImpl(Size newDimensions);

protected:
	Size dimensions;
	PixelFormat format;
};

typedef RefPtr<RenderBuffer> RenderBufferPtr;

} /* namespace nextar */

#endif /* RENDERBUFFER_H_ */
