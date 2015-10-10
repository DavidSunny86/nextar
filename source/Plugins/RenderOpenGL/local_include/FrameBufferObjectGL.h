/*
 * FrameBufferObjGL.h
 *
 *  Created on: 31-May-2014
 *      Author: obhi
 */

#ifndef FRAMEBUFFEROBJGL_H_
#define FRAMEBUFFEROBJGL_H_

#include <RenderOpenGL.h>

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

protected:

	friend class RenderContext_Base_GL;
	static GLenum attachment[(uint32)FrameBuffer::FBTYPE_COUNT];
	GLuint frameBufferObject;
};

} /* namespace RenderOpenGL */

#endif /* FRAMEBUFFEROBJGL_H_ */
