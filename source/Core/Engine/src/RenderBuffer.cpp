/*
 * RenderBuffer.cpp
 *
 *  Created on: 30-May-2014
 *      Author: obhi
 */

#include <RenderBuffer.h>

namespace nextar {

RenderBuffer::RenderBuffer() :
		ContextObject(ContextObject::TYPE_RENDER_BUFFER), format(
				PixelFormat::UNKNOWN), width(0), height(0) {
}

RenderBuffer::~RenderBuffer() {
}

void RenderBuffer::Create(PixelFormat format, uint32 width, uint32 height,
		uint32 samples) {
	this->format = format;
	this->width = width;
	this->height = height;

	CreateParams params;
	params.width = width;
	params.height = height;
	params.format = format;
	params.samples = samples;
	RequestUpdate(MSG_RB_CREATE,
			static_cast<ContextObject::ContextParamPtr>(&params));
}

void RenderBuffer::Capture(RenderContext* rc, PixelBox& image,
		FrameBuffer frameBuffer) {
	View* v = static_cast<View*>(rc->GetView(this));
	v->Update(rc, MSG_RB_READ, &image);
}

} /* namespace nextar */
