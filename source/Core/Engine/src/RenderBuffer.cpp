/*
 * RenderBuffer.cpp
 *
 *  Created on: 30-May-2014
 *      Author: obhi
 */

#include <RenderBuffer.h>
#include <RenderContext.h>

namespace nextar {

RenderBuffer::RenderBuffer() :
		ContextObject(ContextObject::TYPE_RENDER_BUFFER, 0)
		,RenderTarget(RenderTargetType::RENDER_BUFFER)
		,format(PixelFormat::UNKNOWN)
		,dimensions(0, 0) {
}

RenderBuffer::~RenderBuffer() {
}

void RenderBuffer::Create(const CreateParams& params) {
	this->format = params.format;
	this->dimensions = params.dimensions;

	RequestUpdate(MSG_RB_CREATE,
			static_cast<ContextObject::ContextParamPtr>(&params));
}

void RenderBuffer::Capture(RenderContext* rc, PixelBox& image,
		FrameBuffer frameBuffer) {
	View* v = static_cast<View*>(rc->GetView(this));
	v->Update(rc, MSG_RB_READ, &image);
}

Size RenderBuffer::GetDimensions() const {
	return dimensions;
}

PixelFormat RenderBuffer::GetPixelFormat() const {
	return format;
}

void RenderBuffer::ResizeImpl(Size newDimensions) {
	RequestUpdate(MSG_RB_RESIZE,
				static_cast<ContextObject::ContextParamPtr>(&newDimensions));
}


} /* namespace nextar */
