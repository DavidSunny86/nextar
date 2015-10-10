/**
 * Created on: 4th Oct 15
 * @author: obhi
**/
#include "BaseRenderContext.h"
#include "RenderContext_4_0_GL.h"

namespace RenderOpenGL {


RenderContextGL_4_0_GL::RenderContextGL_4_0_GL(RenderDriverGL*) {
}

void RenderContextGL_4_0_GL::InitializeExtensions() {
  ParentContext::InitializeFunctionPointers();
  FunctionTable::InitializeFunctionPointers();
}

}
