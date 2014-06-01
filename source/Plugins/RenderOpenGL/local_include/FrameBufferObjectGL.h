/*
 * FrameBufferObjGL.h
 *
 *  Created on: 31-May-2014
 *      Author: obhi
 */

#ifndef FRAMEBUFFEROBJGL_H_
#define FRAMEBUFFEROBJGL_H_

namespace RenderOpenGL {

	class RenderTextureViewGL;
	/**
	 * Represents a frame buffer object.
	 */
	class FrameBufferObjectGL {
		NEX_LOG_HELPER(FrameBufferObjectGL);
	public:
		FrameBufferObjectGL();
		virtual ~FrameBufferObjectGL();

		bool IsValid() const {
			return frameBufferObject != 0;
		}

		void CreateAndBind(RenderContextGL* gl) {
			if (!frameBufferObject)
				gl->GlGenFramebuffers(1, &frameBufferObject);
			Bind(false, gl);
		}

		void Bind(bool readBuffer, RenderContextGL* gl) {
			gl->GlBindFramebuffer(readBuffer?
					GL_READ_FRAMEBUFFER :
					GL_DRAW_FRAMEBUFFER, frameBufferObject);
		}

		static void Unbind(bool readBuffer, RenderContextGL* gl) {
			gl->GlBindFramebuffer(readBuffer?
							GL_READ_FRAMEBUFFER :
							GL_DRAW_FRAMEBUFFER, 0);
		}

		void Destroy(RenderContextGL* gl) {
			gl->GlDeleteFramebuffers(1, &frameBufferObject);
			frameBufferObject = 0;
		}

		static bool Validate(RenderContextGL* gl);
		void AttachColor(RenderContextGL* gl, uint32 index, RenderTextureViewGL* rt);
		void AttachDepth(RenderContextGL* gl, GLenum type, RenderTextureViewGL* rt);

	protected:

		GLuint frameBufferObject;
	};

} /* namespace RenderOpenGL */

#endif /* FRAMEBUFFEROBJGL_H_ */
