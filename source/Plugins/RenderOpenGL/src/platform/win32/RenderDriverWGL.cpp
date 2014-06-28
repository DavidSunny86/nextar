#include <RenderOpenGL.h>
#include <RenderDriverGL.h>
#include <RenderDriverWGL.h>
#include <RenderContextGLX.h>

namespace RenderOpenGL {

RenderDriverWGL::RenderDriverWGL(int32 gpuIndex) :
		RenderDriverGL(gpuIndex) {
}

RenderDriverWGL::~RenderDriverWGL() {
	renderContexts.clear();
}

RenderContextGL* RenderDriverWGL::CreateContextImpl() {
	return NEX_NEW(RenderContextGLX(this));
}

void* RenderDriverGL::GetExtension(const char* name) {
	size_t len = std::strlen(name);
	if (len > 3 && !std::strcmp((name + len - 3), "ARB"))
		return (void*) glXGetProcAddressARB((const GLubyte *) name);
	return (void*) glXGetProcAddress((const GLubyte *) name);
}

RenderDriverGL* RenderDriverGL::CreateDriverGL(int32 gpuIndex) {
	return NEX_NEW(RenderDriverWGL(gpuIndex));
}
}
