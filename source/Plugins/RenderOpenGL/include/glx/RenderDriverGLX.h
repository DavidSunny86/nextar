
#pragma once

#include <RenderOpenGL.h>
#include <RenderDriverGL.h>

namespace RenderOpenGL {

	class RenderDriverGLX : public RenderDriverGL {
		
		NEX_LOG_HELPER(RenderDriverGLX);

	public:
		RenderDriverGLX(int32 gpuIndex);
		virtual ~RenderDriverGLX();


	protected:
		virtual RenderContextGL* CreateContextImpl();

	};
}
