/*
 * FrameBufferObjGL.cpp
 *
 *  Created on: 31-May-2014
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <FrameBufferObjectGL.h>
#include <RenderContextGL.h>
#include <RenderBufferViewGL.h>
#include <RenderTextureViewGL.h>

namespace RenderOpenGL {

GLenum FrameBufferObjectGL::attachment[] = {
		GL_NONE,
		GL_FRONT_LEFT,
		GL_FRONT_RIGHT,
		GL_BACK_LEFT,
		GL_BACK_RIGHT,
		GL_DEPTH,
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
		GL_COLOR_ATTACHMENT6,
		GL_COLOR_ATTACHMENT7,
		GL_STENCIL_ATTACHMENT,
};

FrameBufferObjectGL::~FrameBufferObjectGL() {
}

void FrameBufferObjectGL::BindNamed(bool readBuffer, FrameBuffer fb, RenderContextGL* gl) {
	NEX_ASSERT(fb >= FrameBuffer::COLOR_0 && fb <= FrameBuffer::COLOR_7);
	if (readBuffer) {
		if (gl->GlNamedFramebufferReadBuffer)
			gl->GlNamedFramebufferReadBuffer(frameBufferObject, attachment[fb]);
		else {
			Bind(true, gl);
			gl->SetReadBuffer(attachment[fb]);
		}
	}
	else {
		if (gl->GlNamedFramebufferDrawBuffer)
			gl->GlNamedFramebufferDrawBuffer(frameBufferObject, attachment[fb]);
		else {
			Bind(false, gl);
			gl->SetDrawBuffer(attachment[fb]);
		}
	}
	GL_CHECK();
}

void FrameBufferObjectGL::UnbindNamed(bool readBuffer, FrameBuffer fb, RenderContextGL* gl) {
	NEX_ASSERT(fb >= FrameBuffer::COLOR_0 && fb <= FrameBuffer::COLOR_7);
	if (readBuffer) {
		if (!gl->GlNamedFramebufferReadBuffer)
			Unbind(true, gl);
		gl->SetReadBuffer(GL_BACK);
	}
	else {
		if (!gl->GlNamedFramebufferDrawBuffer)
			Unbind(false, gl);
		gl->SetDrawBuffer(GL_BACK);
	}
	GL_CHECK();
}

void FrameBufferObjectGL::Attach(RenderContextGL* gl, GLenum attachmentType,
		RenderBufferViewGL* texture) {
	gl->GlFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType,
	GL_RENDERBUFFER, texture->GetRenderBuffer());
}

void FrameBufferObjectGL::Attach(RenderContextGL* gl, GLenum type,
		RenderTextureViewGL* texture) {
	switch (texture->GetType()) {
	case GL_TEXTURE_1D:
		gl->GlFramebufferTexture1D(GL_FRAMEBUFFER, type, GL_TEXTURE_1D,
				texture->GetTexture(), 0);
		break;
	case GL_TEXTURE_2D:
		gl->GlFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D,
				texture->GetTexture(), 0);
		break;
	case TextureBase::TEXTURE_3D:
	case TextureBase::TEXTURE_1D_ARRAY:
	case TextureBase::TEXTURE_2D_ARRAY:
	case TextureBase::TEXTURE_CUBE_MAP:
	case TextureBase::TEXTURE_CUBE_MAP_ARRAY:
		// attach all levels
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	}
}

#define LOG_CASE(x) case x: Error( #x ); return false

bool FrameBufferObjectGL::Validate(RenderContextGL* gl) {
	switch (gl->GlCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER)) {
	LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
;		LOG_CASE( GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT );
		LOG_CASE( GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER );
		LOG_CASE( GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER );
		LOG_CASE( GL_FRAMEBUFFER_UNSUPPORTED );
		LOG_CASE( GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE );
		LOG_CASE( GL_TEXTURE_FIXED_SAMPLE_LOCATIONS );
		LOG_CASE( GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS );
		case GL_FRAMEBUFFER_COMPLETE:
		return true;
	}
	return false;
}
#undef LOG_CASE
}
/* namespace RenderOpenGL */
