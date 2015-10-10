/*
 * RenderTextureViewGL.h
 *
 *  Created on: 31-May-2014
 *      Author: obhi
 */

#ifndef RENDERTEXTUREVIEWGL_H_
#define RENDERTEXTUREVIEWGL_H_

#include <RenderOpenGL.h>
#include <TextureViewGL.h>
#include <FrameBufferObjectGL.h>

namespace RenderOpenGL {

class RenderTextureViewGL: public TextureViewGL {
public:
	RenderTextureViewGL();
	virtual ~RenderTextureViewGL();

	virtual void Update(RenderContext*, uint32, ContextObject::ContextParamPtr);
	virtual void Destroy(RenderContext*);

/*
	inline void CreateFBO(RenderContextGL* gl) {
		fbo.Create(gl);
		fbo.Bind(false, gl);
		fbo.Attach(gl, pixelFormat.attachmentType + 0, this);
		FrameBufferObjectGL::Validate(gl);
		fbo.Unbind(false, gl);
	}

	inline void DestroyFBO(RenderContextGL* gl) {
		fbo.Destroy(gl);
	}
*/
	FrameBufferObjectGL& GetFBO() {
		return fbo;
	}

protected:

	FrameBufferObjectGL fbo;
};

} /* namespace nextar */

#endif /* RENDERTEXTUREVIEWGL_H_ */
