/*
 * RenderBufferViewGL.cpp
 *
 *  Created on: 13-Jun-2014
 *      Author: obhi
 */

#include <RenderBufferViewGL.h>
#include <RenderContextGL.h>

namespace RenderOpenGL {

RenderBufferViewGL::RenderBufferViewGL() :
		renderBuffer(0), attachmentType(GL_NONE) {
}

RenderBufferViewGL::~RenderBufferViewGL() {
}

void RenderBufferViewGL::Update(nextar::RenderContext* rc, uint32 msg,
		ContextObject::ContextParamPtr param) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	if (msg == RenderBuffer::MSG_RB_CREATE) {
		const RenderBuffer::CreateParams* createParams =
				static_cast<const RenderBuffer::CreateParams*>(param);
		gl->GlGenRenderbuffers(1, &renderBuffer);
		gl->GlBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
		PixelFormatGl format = RenderContextGL::GetGlPixelFormat(
				createParams->format, createParams->format);
		attachmentType = format.attachmentType;
		gl->GlRenderbufferStorageMultisample(GL_RENDERBUFFER,
				createParams->samples, format.internalFormat,
				createParams->width, createParams->height);
		gl->GlBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}

void RenderBufferViewGL::Destroy(RenderContext* rc) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	gl->GlDeleteRenderbuffers(1, &renderBuffer);
}
} /* namespace RenderOpenGL */
