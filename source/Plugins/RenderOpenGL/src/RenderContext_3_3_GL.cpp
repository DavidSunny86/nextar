/**
 * Created on: 4th Oct 15
 * @author: obhi
**/
#include "BaseRenderContext.h"
#include "RenderContext_3_3_GL.h"

namespace RenderOpenGL {


RenderContextGL_3_3_GL::RenderContextGL_3_3_GL(RenderDriverGL*) {
}

void RenderContextGL_3_3_GL::InitializeExtensions() {
  ParentContext::InitializeFunctionPointers();
  FunctionTable::InitializeFunctionPointers();
}

}
