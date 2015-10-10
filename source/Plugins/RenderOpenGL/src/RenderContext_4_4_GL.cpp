/**
 * Created on: 4th Oct 15
 * @author: obhi
**/
#include "BaseRenderContext.h"
#include "RenderContextGL_4_4_GL.h"

namespace RenderOpenGL {


RenderContextGL_4_4_GL::RenderContextGL_4_4_GL(RenderDriverGL*) {
}

void RenderContextGL_4_4_GL::InitializeExtensions() {
  ParentContext::InitializeFunctionPointers();
  FunctionTable::InitializeFunctionPointers();
}

}
