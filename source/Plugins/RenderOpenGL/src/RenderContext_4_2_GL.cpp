/**
 * Created on: 4th Oct 15
 * @author: obhi
**/
#include "BaseRenderContext.h"
#include "RenderContextGL_4_2_GL.h"

namespace RenderOpenGL {


RenderContextGL_4_2_GL::RenderContextGL_4_2_GL(RenderDriverGL*) {
}

void RenderContextGL_4_2_GL::InitializeExtensions() {
  ParentContext::InitializeFunctionPointers();
  FunctionTable::InitializeFunctionPointers();
}

}
