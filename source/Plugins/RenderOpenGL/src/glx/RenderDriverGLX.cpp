
#include "RenderOpenGL.h"
#include "RenderDriverGL.h"
#include "glx/RenderDriverGLX.h"
#include "glx/RenderContextGLX.h"

namespace RenderOpenGL {


	RenderDriverGLX::RenderDriverGLX(int32 gpuIndex) : RenderDriverGL(gpuIndex) {
	}

	RenderDriverGLX::~RenderDriverGLX() {
		renderContexts.clear();
	}

	RenderContextGL* RenderDriverGLX::CreateContextImpl() {
		return NEX_NEW RenderContextGLX(this);
	}

	void* RenderDriverGL::GetExtension(const char* name) {
		size_t len = std::strlen(name);
		if (len > 3 && !std::strcmp((name + len-3), "ARB"))
			return (void*)glXGetProcAddressARB((const GLubyte *)name);
		return (void*)glXGetProcAddress((const GLubyte *)name);
	}

	RenderDriverGL* RenderDriverGL::CreateDriverGL(int32 gpuIndex) {
		return NEX_NEW RenderDriverGLX(gpuIndex);
	}
}
