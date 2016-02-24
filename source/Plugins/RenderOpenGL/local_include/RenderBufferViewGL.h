/*
 * RenderBufferViewGL.h
 *
 *  Created on: 13-Jun-2014
 *      Author: obhi
 */

#ifndef RENDERBUFFERVIEWGL_H_
#define RENDERBUFFERVIEWGL_H_

#include <RenderOpenGL.h>
#include <RenderBuffer.h>
#include <FrameBufferObjectGL.h>

namespace RenderOpenGL {

/*
 * Render buffers
 */
class RenderBufferViewGL: public ContextObject::View {
public:
	RenderBufferViewGL();
	virtual ~RenderBufferViewGL();

	virtual void Update(nextar::RenderContext*, uint32 msg,
			ContextObject::ContextParamPtr);
	virtual void Destroy(nextar::RenderContext*);

	GLenum GetAttachment() const {
		return attachmentType;
	}

	inline GLuint GetRenderBuffer() const {
		return renderBuffer;
	}

/*	inline void CreateFBO(RenderContext_Base_GL* gl) {
		fbo.Create(gl);
		fbo.Bind(false, gl);
		fbo.Attach(gl, attachmentType + 0, this);
		FrameBufferObjectGL::Validate(gl);
		fbo.Unbind(false, gl);
	}

	inline void DestroyFBO(RenderContext_Base_GL* gl) {
		fbo.Destroy(gl);
	}
*/
	FrameBufferObjectGL& GetFBO() {
		return fbo;
	}

	bool IsColorTarget() const {
		return attachmentType != GL_DEPTH_ATTACHMENT &&
			attachmentType != GL_DEPTH_STENCIL_ATTACHMENT;
	}

	bool IsDepthStencilTarget() const {
		return !IsColorTarget();
	}

protected:
	friend class RenderContext_Base_GL;
	PixelFormatGl format;
	FrameBufferObjectGL fbo;
	uint16 samples;
	GLenum attachmentType;
	GLuint renderBuffer;
};

} /* namespace RenderOpenGL */

#endif /* RENDERBUFFERVIEWGL_H_  */
