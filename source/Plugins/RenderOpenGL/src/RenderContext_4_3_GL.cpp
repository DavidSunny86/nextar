/**
 * Created on: 4th Oct 15
 * @author: obhi
**/
#include "BaseRenderContext.h"
#include "RenderContextGL_4_3_GL.h"

namespace RenderOpenGL {


static void APIENTRY ReportError(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	void* userParam) {
	reinterpret_cast<RenderContextGL*>(userParam)->ReportError(source, type, id, severity, length, message);
}

RenderContextGL_4_3_GL::RenderContextGL_4_3_GL(RenderDriverGL*) {
}

void RenderContextGL_4_3_GL::InitializeExtensions() {
  ParentContext::InitializeFunctionPointers();
  FunctionTable::InitializeFunctionPointers();
}

void RenderContextGL_4_3_GL::PostWindowCreation(RenderWindow* gw) {
  RenderContextGL_4_2_GL::PostWindowCreation(gw);
#if defined (NEX_DEBUG)
	// attach debug hook
  GlDebugMessageCallback((GLDEBUGPROC)RenderOpenGL::ReportError, this);
#endif
}

}
