/*
 * MultiRenderTargetViewGL.h
 *
 *  Created on: 13-Jun-2014
 *      Author: obhi
 */

#ifndef MULTIRENDERTARGETVIEWGL_H_
#define MULTIRENDERTARGETVIEWGL_H_

#include <RenderOpenGL.h>
#include <MultiRenderTarget.h>
#include <FrameBufferObjectGL.h>

namespace RenderOpenGL {

/*
 *
 */
class MultiRenderTargetViewGL: public MultiRenderTarget::View {
public:

	MultiRenderTargetViewGL();
	virtual ~MultiRenderTargetViewGL();

	virtual void Update(nextar::RenderContext*, uint32 msg,
			ContextObject::ContextParamPtr);
	virtual void Destroy(nextar::RenderContext*);

	FrameBufferObjectGL& GetFBO() {
		return fbo;
	}

protected:

	FrameBufferObjectGL fbo;
};

} /* namespace RenderOpenGL */

#endif /* MULTIRENDERTARGETVIEWGL_H_ */
