/**
 * Created on: 4th Oct 15
 * @author: obhi
 **/
#include <BaseRenderContext.h>
#include <RenderContext_4_1_GL.h>

namespace RenderOpenGL {


RenderContext_4_1_GL::RenderContext_4_1_GL(RenderDriverGL* d) : ParentContext(d) {
}

void RenderContext_4_1_GL::InitializeExtensions() {
	ParentContext::InitializeExtensions();
	FunctionTable::InitializeFunctionPointers();
}

VersionGL RenderContext_4_1_GL::GetContextVersion() {
	return GLV_4_1;
}

}
