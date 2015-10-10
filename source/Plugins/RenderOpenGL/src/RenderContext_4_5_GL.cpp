/**
 * Created on: 4th Oct 15
 * @author: obhi
**/
#include "BaseRenderContext.h"
#include "RenderContextGL_4_5_GL.h"

namespace RenderOpenGL {


RenderContextGL_4_5_GL::RenderContextGL_4_5_GL(RenderDriverGL*) {
}

void RenderContextGL_4_5_GL::InitializeExtensions() {
  ParentContext::InitializeFunctionPointers();
  FunctionTable::InitializeFunctionPointers();
}

void RenderContextGL_4_5_GL::BindNamedFBO(FrameBufferObjectGL& fbo,
  bool readBuffer, FrameBuffer fb) {
  NEX_ASSERT(fb >= FrameBuffer::COLOR_0 && fb <= FrameBuffer::COLOR_7);
	if (readBuffer) {
		GlNamedFramebufferReadBuffer(fbo.frameBufferObject, s_attachmentMap[(uint32)fb]);
	}	else {
		GlNamedFramebufferDrawBuffer(fbo.frameBufferObject, s_attachmentMap[(uint32)fb]);
	}
	GL_CHECK();
}

void RenderContextGL_4_5_GL::UnbindNamedFBO(
  bool readBuffer, FrameBuffer fb) {
	NEX_ASSERT(fb >= FrameBuffer::COLOR_0 && fb <= FrameBuffer::COLOR_7);
	if (readBuffer) {
		SetReadBuffer(GL_BACK);
	}
	else {
		SetDrawBuffer(GL_BACK);
	}
	GL_CHECK();
}

}
