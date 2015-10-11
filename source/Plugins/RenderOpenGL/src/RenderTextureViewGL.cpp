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
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	gl->DestroyFBO(fbo);
	TextureViewGL::Destroy(rc);
}
} /* namespace nextar */
