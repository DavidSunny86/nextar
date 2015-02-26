/*
 * FrameBufferObjGL.h
 *
 *  Created on: 31-May-2014
 *      Author: obhi
 */

#ifndef FRAMEBUFFEROBJGL_H_
#define FRAMEBUFFEROBJGL_H_

#include <RenderContextGL.h>

namespace RenderOpenGL {

class RenderTextureViewGL;
class RenderBufferViewGL;
/**
 * Represents a frame buffer object.
 */
class FrameBufferObjectGL {
	NEX_LOG_HELPER(FrameBufferObjectGL)
	;
public:

	FrameBufferObjectGL() : frameBufferObject(0) {};
	virtual ~FrameBufferObjectGL();

	bool IsValid() const {
		return frameBufferObject != 0;
	}

	void Create(RenderContextGL* gl) {
		if (!frameBufferObject)
			gl->GlGenFramebuffers(1, &frameBufferObject);
	}

	void Bind(bool readBuffer, RenderContextGL* gl) {
		gl->GlBindFramebuffer(readBuffer ?
		GL_READ_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER,
				frameBufferObject);
	}

	void BindNamed(bool readBuffer, FrameBuffer fb, RenderContextGL* gl);
	void UnbindNamed(bool readBuffer, FrameBuffer fb, RenderContextGL* gl);

	static void Unbind(bool readBuffer, RenderContextGL* gl) {
		gl->GlBindFramebuffer(readBuffer ?
		GL_READ_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);
	}

	void Destroy(RenderContextGL* gl) {
		gl->GlDeleteFramebuffers(1, &frameBufferObject);
		frameBufferObject = 0;
	}

	static bool Validate(RenderContextGL* gl);

	void Attach(RenderContextGL* gl, GLenum attachmentType,
			RenderTextureViewGL* rt);
	void Attach(RenderContextGL* gl, GLenum attachmentType,
			RenderBufferViewGL* rt);

protected:

	friend class RenderContextGL;
	static GLenum attachment[(uint32)FrameBuffer::FBTYPE_COUNT];
	GLuint frameBufferObject;
};

} /* namespace RenderOpenGL */

#endif /* FRAMEBUFFEROBJGL_H_ */
