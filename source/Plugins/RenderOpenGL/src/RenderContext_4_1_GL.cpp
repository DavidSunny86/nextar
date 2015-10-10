/**
 * Created on: 4th Oct 15
 * @author: obhi
**/
#include "BaseRenderContext.h"
#include "RenderContextGL_4_1_GL.h"

namespace RenderOpenGL {


RenderContextGL_4_1_GL::RenderContextGL_4_1_GL(RenderDriverGL*) {
}

void RenderContextGL_4_1_GL::InitializeExtensions() {
  ParentContext::InitializeFunctionPointers();
  FunctionTable::InitializeFunctionPointers();
}

}
