/**
 * Created on: 4th Oct 15
 * @author: obhi
 **/
#include "BaseRenderContext.h"
#include "RenderContext_4_5_GL.h"

namespace RenderOpenGL {


RenderContext_4_5_GL::RenderContext_4_5_GL(RenderDriverGL* d) : ParentContext(d) {
}

void RenderContext_4_5_GL::InitializeExtensions() {
	ParentContext::InitializeExtensions();
	FunctionTable::InitializeFunctionPointers();
}

void RenderContext_4_5_GL::BindNamedFBO(FrameBufferObjectGL& fbo,
										bool readBuffer, FrameBuffer fb) {
	NEX_ASSERT(fb >= FrameBuffer::COLOR_0 && fb <= FrameBuffer::COLOR_7);
	if (readBuffer) {
		GlNamedFramebufferReadBuffer(fbo.GetFBO(), s_attachmentMap[(uint32)fb]);
	} else {
		GlNamedFramebufferDrawBuffer(fbo.GetFBO(), s_attachmentMap[(uint32)fb]);
	}
	GL_CHECK();
}

void RenderContext_4_5_GL::UnbindNamedFBO(bool readBuffer) {
	if (readBuffer) {
		SetReadBuffer(GL_BACK);
	} else {
		SetDrawBuffer(GL_BACK);
	}
	GL_CHECK();
}

VersionGL RenderContext_4_5_GL::GetContextVersion() {
	return GLV_4_5;
}

}
