/*
 * RenderTextureViewGL.cpp
 *
 *  Created on: 31-May-2014
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <RenderTextureViewGL.h>

namespace RenderOpenGL {

	RenderTextureViewGL::RenderTextureViewGL() : fbo(0) {
	}

	RenderTextureViewGL::~RenderTextureViewGL() {
	}

	void RenderTextureViewGL::Update(
			RenderContext* rc, uint32 msg,
			ContextObject::ContextParamPtr cb) {
		if (msg & TextureBase::MSG_TEX_READ)
			NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
		else
			TextureViewGL::Update(rc, msg, cb);
	}


} /* namespace nextar */
