/**
 * Created on: 4th Oct 15
 * @author: obhi
 **/
#include "BaseRenderContext.h"
#include "RenderContext_4_4_GL.h"

namespace RenderOpenGL {


RenderContext_4_4_GL::RenderContext_4_4_GL(RenderDriverGL* d) : ParentContext(d) {
}

void RenderContext_4_4_GL::InitializeExtensions() {
	ParentContext::InitializeExtensions();
	FunctionTable::InitializeFunctionPointers();
}

VersionGL RenderContext_4_4_GL::GetContextVersion() {
	return GLV_4_4;
}

}
