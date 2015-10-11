/**
 * Created on: 4th Oct 15
 * @author: obhi
**/
#include "BaseRenderContext.h"
#include "RenderContext_4_3_GL.h"

namespace RenderOpenGL {


static void APIENTRY ReportError(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	void* userParam) {
	reinterpret_cast<RenderContext_Base_GL*>(userParam)->ReportError(source, type, id, severity, length, message);
}

RenderContext_4_3_GL::RenderContext_4_3_GL(RenderDriverGL* d) : ParentContext(d) {
}

void RenderContext_4_3_GL::InitializeExtensions() {
  ParentContext::InitializeExtensions();
  FunctionTable::InitializeFunctionPointers();
}

void RenderContext_4_3_GL::PostWindowCreation(RenderWindow* gw) {
  ParentContext::PostWindowCreation(gw);
#if defined (NEX_DEBUG)
	// attach debug hook
  GlDebugMessageCallback((GLDEBUGPROC)RenderOpenGL::ReportError, this);
#endif
}

VersionGL RenderContext_4_3_GL::GetContextVersion() {
	return GLV_4_3;
}

}
