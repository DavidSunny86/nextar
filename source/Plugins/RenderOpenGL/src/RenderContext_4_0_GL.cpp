/**
 * Created on: 4th Oct 15
 * @author: obhi
 **/
#include <BaseRenderContext.h>
#include <RenderContext_4_0_GL.h>

namespace RenderOpenGL {


RenderContext_4_0_GL::RenderContext_4_0_GL(RenderDriverGL* d) : ParentContext(d) {
}

void RenderContext_4_0_GL::InitializeExtensions() {
	ParentContext::InitializeExtensions();
	FunctionTable::InitializeFunctionPointers();
}

VersionGL RenderContext_4_0_GL::GetContextVersion() {
	return GLV_4_0;
}

}
