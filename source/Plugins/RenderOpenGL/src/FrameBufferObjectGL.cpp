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

}
/* namespace RenderOpenGL */
