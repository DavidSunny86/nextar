/*
 * RenderTextureViewGL.cpp
 *
 *  Created on: 31-May-2014
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <RenderTextureViewGL.h>

namespace RenderOpenGL {

RenderTextureViewGL::RenderTextureViewGL() :
		fbo() {
}

RenderTextureViewGL::~RenderTextureViewGL() {
}

void RenderTextureViewGL::Update(RenderContext* rc, uint32 msg,
		ContextObject::ContextParamPtr cb) {
	TextureViewGL::Update(rc, msg, cb);
}

void RenderTextureViewGL::Destroy(RenderContext* rc) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	fbo.Destroy(gl);
	TextureViewGL::Destroy(rc);
}
} /* namespace nextar */
