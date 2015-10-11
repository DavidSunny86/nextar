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
		renderBuffer(0), attachmentType(GL_NONE) {
}

RenderBufferViewGL::~RenderBufferViewGL() {
}

void RenderBufferViewGL::Update(nextar::RenderContext* rc, uint32 msg,
		ContextObject::ContextParamPtr param) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	if (msg == RenderBuffer::MSG_RB_CREATE) {
		const RenderBuffer::CreateParams* createParams =
				static_cast<const RenderBuffer::CreateParams*>(param);
		PixelFormatGl format = RenderContext_Base_GL::GetGlPixelFormat(
				createParams->format, createParams->format);
		attachmentType = format.attachmentType;
		renderBuffer = gl->CreateRenderBuffer(createParams, format);
	}
}

void RenderBufferViewGL::Destroy(RenderContext* rc) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	gl->DestroyRenderBuffer(renderBuffer);
}
} /* namespace RenderOpenGL */
