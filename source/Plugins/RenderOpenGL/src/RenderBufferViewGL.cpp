/*
 * RenderBufferViewGL.cpp
 *
 *  Created on: 13-Jun-2014
 *      Author: obhi
 */

#include <RenderBufferViewGL.h>
#include <RenderContext_Base_GL.h>

namespace RenderOpenGL {

RenderBufferViewGL::RenderBufferViewGL() :
		renderBuffer(0),
		attachmentType(GL_NONE),
		samples(0) {
}

RenderBufferViewGL::~RenderBufferViewGL() {
}

void RenderBufferViewGL::Update(nextar::RenderContext* rc, uint32 msg,
		ContextObject::ContextParamPtr param) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	if (msg == RenderBuffer::MSG_RB_CREATE) {
		const RenderBuffer::CreateParams* createParams =
				static_cast<const RenderBuffer::CreateParams*>(param);
		format = RenderContext_Base_GL::GetGlPixelFormat(
				createParams->format, createParams->format);
		samples = createParams->samples;
		attachmentType = format.attachmentType;
		renderBuffer = gl->CreateRenderBuffer(createParams->dimensions, samples, format);
	} else if (msg == RenderBuffer::MSG_RB_RESIZE) {
		const Size& s = *reinterpret_cast<const Size*>(param);
		gl->DestroyRenderBuffer(renderBuffer);
		renderBuffer = gl->CreateRenderBuffer(s, samples, format);
	}
}

void RenderBufferViewGL::Destroy(RenderContext* rc) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	gl->DestroyRenderBuffer(renderBuffer);
}
} /* namespace RenderOpenGL */
