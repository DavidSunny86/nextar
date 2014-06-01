/*
 * FrameBufferObjGL.cpp
 *
 *  Created on: 31-May-2014
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <FrameBufferObjectGL.h>

namespace RenderOpenGL {

	FrameBufferObjectGL::~FrameBufferObjectGL() : frameBufferObject(0) {
	}

	void FrameBufferObjectGL::AttachColor(RenderContextGL* gl, uint32 index, RenderTextureViewGL* texture) {
		switch(texture->GetType()) {
		case GL_TEXTURE_1D:
			gl->GlFramebufferTexture1D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_1D, texture->GetTexture(), 0);
			break;
		case GL_TEXTURE_2D:
			gl->GlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture->GetTexture(), 0);
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

	void FrameBufferObjectGL::AttachDepth(RenderContextGL* gl, GLenum type, RenderTextureViewGL* texture) {
		switch(texture->GetType()) {
		case GL_TEXTURE_1D:
			gl->GlFramebufferTexture1D(GL_FRAMEBUFFER, type, GL_TEXTURE_1D, texture->GetTexture(), 0);
			break;
		case GL_TEXTURE_2D:
			gl->GlFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D, texture->GetTexture(), 0);
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
		switch(gl->GlCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER)) {
		LOG_CASE( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT );
		LOG_CASE( GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT );
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
} /* namespace RenderOpenGL */
